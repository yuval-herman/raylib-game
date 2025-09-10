#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

#include <threads.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "trainer.h"

typedef struct Individual
{
    genann *brain;
    float fitness;
} Individual;

#define TREND_WINDOW 10

typedef struct TrainingStats
{
    float alltime_max_fit, max_fit, min_fit, avg_fit, fit_trend;
    float trend_window[TREND_WINDOW];
    size_t trend_i;
} TrainingStats;

// Here we store all mutexes, condition, and other crap needed to sync between threads.
// This struct should be initialized once and passed by pointer.
typedef struct ThreadsSync
{
    int thread_amount;
    mtx_t shared_mtx;
    cnd_t cond_workers;
    cnd_t cond_main;
    int finished_count;
    int epoch;
    bool quit;
} ThreadsSync;

// This is passed to threads
typedef struct ThreadData
{
    Creature creature;
    genann *population;
    float *fitnesses;
    size_t start_index;
    size_t end_index;

    // Thread synchronization shenanigans
    int thread_amount;
    mtx_t *shared_mtx;
    cnd_t *cond_workers;
    cnd_t *cond_main;
    int *finished_count;
    int *epoch; // current epoch number (generation)
    bool *quit; // exit flag set by main
} ThreadData;

TrainingStats make_training_stats()
{
    return (TrainingStats){
        .alltime_max_fit = -INFINITY,
        .max_fit = -INFINITY,
        .min_fit = INFINITY,
        .avg_fit = 0,
        .fit_trend = 0,
        .trend_i = 0,
        .trend_window = {0},
    };
}

// Number of CPU cores on the machine
int n_cores(void)
{
#if defined(ENABLE_THREADS) && ENABLE_THREADS
#ifdef _WIN32
    SYSTEM_INFO siSysInfo;
    GetSystemInfo(&siSysInfo);
    return siSysInfo.dwNumberOfProcessors;
#else
    return sysconf(_SC_NPROCESSORS_ONLN);
#endif
#else
    return 1;
#endif
}

// Get average node position
b2Vec2 get_avg_position(Creature *creature)
{
    b2Vec2 avg = {0};
    for (size_t body_i = 0; body_i < creature->node_amount; body_i++)
    {
        b2Vec2 pos = b2Body_GetPosition(creature->node_ids[body_i]);
        avg.x += pos.x;
        avg.y += pos.y;
    }
    avg.x /= creature->node_amount;
    avg.y /= creature->node_amount;
    return avg;
}

genann *init_population(RandomState *rng, Creature *creature)
{
    genann *ann = creature->brain;

    const int weights_size = sizeof(double) * (ann->total_weights + ann->total_neurons + (ann->total_neurons - ann->inputs));
    const int size = sizeof(genann) + weights_size;
    // Allocate continuos memory block for all ann and their weights with this layout:
    // [genann structs][weights blocks indexed by i]
    void *population = malloc(size * POP_SIZE);

    // Pointer with 1 byte alignment to the begining of the weights section
    char *weights_start = (char *)(((genann *)population) + POP_SIZE);
    for (size_t i = 0; i < POP_SIZE; i++)
    {
        ((genann *)population)[i] = *ann;
        /* Set pointers. */
        ((genann *)population)[i].weight = (double *)(weights_start + weights_size * i);
        ((genann *)population)[i].output = ((genann *)population)[i].weight + ((genann *)population)[i].total_weights;
        ((genann *)population)[i].delta = ((genann *)population)[i].output + ((genann *)population)[i].total_neurons;

        genann_randomize(rng, ((genann *)population) + i);
    }

    return population;
}
void free_population(genann *population)
{
    // Since I am a psychopath, the entire population is
    // a single memory block and can be free'd with one free call.
    // You're welcome
    free(population);
}

float evaluate(RandomState *rng, Creature *creature)
{
    // TODO: I'm ignoring UP for now since it's a bit complicated
    CreatureInstruction inst_arr[] = {INST_NONE, INST_LEFT, INST_RIGHT};

    float fitness = 0;
    bool early_termination = false;
    int stagnation_steps = 0;
    CreatureInstruction inst;

    creature_reset(creature);
    for (int test = 0; test < EVALUATION_TESTS; test++)
    {
        inst = inst_arr[random_uint64_range(rng, 0, ARRAY_COUNT(inst_arr) - 1)];
        b2Vec2 start_pos = get_avg_position(creature);
        b2Vec2 last_pos = start_pos;

        // I use a random amount of steps so the creature won't get used to fixed input
        int steps = random_uint64_upto(rng, EVALUATION_STEPS);
        for (int i = 0; i < steps; i++)
        {
            creature_think(creature, inst);
            b2World_Step(creature->world_id, TIME_STEP, SUB_STEP_COUNT);
            b2Vec2 pos = get_avg_position(creature);

            early_termination = inst != INST_NONE && (fabsf(last_pos.x - pos.x) < FLT_EPSILON);
            if (early_termination && ++stagnation_steps == EVALUATION_EARLY_TERMINATION_STEPS)
                break;
            else
            {
                early_termination = false;
                stagnation_steps = 0;
            }
            last_pos = pos;
        }

        // Reward for standing up
        if (last_pos.y > creature->node_radius)
        {
            fitness += 5;
        }

        // Penalize for not moving
        if (early_termination)
        {
            fitness -= EVALUATION_EARLY_TERMINATION_PENALTY;
            break;
        }

        float distance = fabsf(last_pos.x - start_pos.x);
        // Penalize for moving while instructed to stop
        if (inst == INST_NONE)
        {
            fitness -= distance * EVALUATION_PENALTY;
        }
        else
        {
            bool moved_right = last_pos.x > start_pos.x;
            bool should_move_right = inst == INST_RIGHT;
            //  Reward for moving in the correct direction
            if (moved_right == should_move_right)
            {
                fitness += distance;
            }
            // Penalize for moving in the wrong direction
            else
                fitness -= distance * EVALUATION_PENALTY;
        }
    }
    return fitness;
}

Individual select(RandomState *rng, genann *population, float *fitnesses)
{
    size_t index_max = random_uint64_range(rng, 0, POP_SIZE - 1);
    for (int i = 0; i < TOURNAMENT_SIZE; i++)
    {
        size_t index_check = random_uint64_range(rng, 0, POP_SIZE - 1);
        if (fitnesses[index_max] < fitnesses[index_check])
            index_max = index_check;
    }
    return (Individual){.brain = population + index_max, .fitness = fitnesses[index_max]};
}

void crossover(RandomState *rng, const Individual stronger, const Individual weaker, genann *child)
{
    assert(stronger.fitness >= weaker.fitness);
    assert(stronger.brain->total_weights == weaker.brain->total_weights);
    for (int i = 0; i < stronger.brain->total_weights; ++i)
    {
        child->weight[i] = random_double(rng) < CROSSOVER_BIAS ? stronger.brain->weight[i] : weaker.brain->weight[i];
    }
}

void mutation(RandomState *rng, genann *ind)
{
    for (int i = 0; i < ind->total_weights; ++i)
    {
        if (random_double(rng) < MUTATION_RATE)
        {
            ind->weight[i] += (random_double(rng) - 0.5) * MAX_MUTATION_AMOUNT;
            ind->weight[i] = b2ClampFloat(ind->weight[i], -0.5, 0.5);
        }
    }
}

// Copy weights from src to dst safely (works when src may have separately allocated weight buffer)
static void copy_weights(const genann *src, genann *dst)
{
    assert(src->total_weights == dst->total_weights);
    assert(src->total_neurons == dst->total_neurons);
    assert(src->inputs == dst->inputs);
    assert(src->hidden_layers == dst->hidden_layers);
    assert(src->hidden == dst->hidden);
    assert(src->outputs == dst->outputs);

    // Copy only the weight buffer. Outputs and deltas are scratch and don't need persistent copy.
    memcpy(dst->weight, src->weight, sizeof(double) * src->total_weights);
}

double crossover_abort_chance(int generation)
{
    const double target_rate = CROSSOVER_ABORT_MAX;
    const double min_rate = CROSSOVER_ABORT_MIN;
    return b2MaxFloat(min_rate, ((double)generation * target_rate) / EVOLUTION_GENERATIONS);
}

int thread_job(void *arg)
{
    RandomState *rng = random_make_seed();
    ThreadData *data = arg;
    genann *old_brain = data->creature.brain;
    int local_epoch;

    TraceLog(LOG_DEBUG, "worker %zu thread started", data->start_index);

    while (true)
    {
        mtx_lock(data->shared_mtx);
        local_epoch = *data->epoch;
        mtx_unlock(data->shared_mtx);
        TraceLog(LOG_DEBUG, "worker %zu thread evaluating population for epoch %d", data->start_index, local_epoch);

        // Actual work, the rest is trying to keep threads from tearing each other apart...
        for (size_t pop_i = data->start_index; pop_i < data->end_index; pop_i++)
        {
            data->creature.brain = data->population + pop_i;
            data->fitnesses[pop_i] = evaluate(rng, &data->creature);
        }

        // Notify main we finished this epoch
        mtx_lock(data->shared_mtx);
        (*data->finished_count)++;

        if (*data->finished_count == data->thread_amount)
        {
            // last worker wakes main
            cnd_signal(data->cond_main);
        }

        // Wait until main advances the epoch (or sets quit)
        while (*data->epoch == local_epoch && !*data->quit)
        {
            // This unlock `shared_mtx` while waiting, and re-locks it when done
            cnd_wait(data->cond_workers, data->shared_mtx);
        }

        int should_quit = *data->quit;
        mtx_unlock(data->shared_mtx);

        if (should_quit)
            break;
    }

    data->creature.brain = old_brain;
    TraceLog(LOG_DEBUG, "worker %zu thread exiting", data->start_index);
    random_destroy(rng);
    return thrd_success;
}

ThreadData *make_threads_data(RandomState *rng,
                              genann *population,
                              float *fitnesses,
                              const Creature *creature,
                              ThreadsSync *thread_sync)
{
    ThreadData *thread_data = malloc(sizeof thread_data[0] * thread_sync->thread_amount);
    const int chunk_size = POP_SIZE / thread_sync->thread_amount;
    for (int i = 0; i < thread_sync->thread_amount; i++)
    {
        int start = i * chunk_size;
        int end = (i == thread_sync->thread_amount - 1) ? POP_SIZE : start + chunk_size;

        thread_data[i].start_index = start;
        thread_data[i].end_index = end;
        thread_data[i].fitnesses = fitnesses;
        thread_data[i].population = population;
        b2WorldId private_world_id = physics_make_world();
        // This will be destroyed when destroying the world so we don't bother storing its id
        physics_make_ground(private_world_id);
        thread_data[i].creature = creature_make(rng, private_world_id, creature->original_node_positions, creature->node_amount, creature->joints_data, creature->joint_amount);

        thread_data[i].thread_amount = thread_sync->thread_amount;
        thread_data[i].shared_mtx = &thread_sync->shared_mtx;
        thread_data[i].cond_workers = &thread_sync->cond_workers;
        thread_data[i].cond_main = &thread_sync->cond_main;
        thread_data[i].finished_count = &thread_sync->finished_count;
        thread_data[i].epoch = &thread_sync->epoch; // current epoch number (generation)
        thread_data[i].quit = &thread_sync->quit;   // exit flag set by main
    }
    return thread_data;
}
void destroy_threads(thrd_t *threads, ThreadData *thread_data, ThreadsSync *thread_sync)
{
    TraceLog(LOG_DEBUG, "Trying to destroy threads. Wish me luck.");
    // Tell workers to quit and wake them
    mtx_lock(&thread_sync->shared_mtx);
    thread_sync->quit = 1;
    cnd_broadcast(&thread_sync->cond_workers);
    mtx_unlock(&thread_sync->shared_mtx);
    TraceLog(LOG_DEBUG, "Told em to stop. Now we wait...");

    // Join threads
    for (int i = 0; i < thread_sync->thread_amount; ++i)
    {
        TraceLog(LOG_DEBUG, "joining worker %d", i);
        thrd_join(threads[i], NULL);
        TraceLog(LOG_DEBUG, "joined worker %d", i);
    }

    for (int i = 0; i < thread_sync->thread_amount; i++)
    {
        b2DestroyWorld(thread_data[i].creature.world_id);
        creature_destroy(&thread_data[i].creature);
    }

    free(thread_data);
    free(threads);
    mtx_destroy(&thread_sync->shared_mtx);
    cnd_destroy(&thread_sync->cond_workers);
    cnd_destroy(&thread_sync->cond_main);
}

// This is critical code that has to be inside a mutex lock!
void wait_all_threads(ThreadsSync *thread_sync)
{
    while (thread_sync->finished_count < thread_sync->thread_amount)
    {
        cnd_wait(&thread_sync->cond_main, &thread_sync->shared_mtx);
    }

    thread_sync->finished_count = 0; // reset for next epoch
    thread_sync->epoch++;            // advance epoch
}

int evaluate_threads(ThreadData *thread_data, thrd_t *threads, ThreadsSync *thread_sync)
{
    /* Create worker threads once. They will loop and wait on condition variable
    between epochs. */
    if (thread_sync->epoch == 0)
    {
        for (int i = 0; i < thread_sync->thread_amount; i++)
        {
            if (thrd_create(&threads[i], thread_job, thread_data + i) != thrd_success)
            {
                TraceLog(LOG_ERROR, "Error creating thread %d.\n", i);
                return 1;
            }
        }
        mtx_lock(&thread_sync->shared_mtx);
        wait_all_threads(thread_sync);
        mtx_unlock(&thread_sync->shared_mtx);
    }
    else
    {
        mtx_lock(&thread_sync->shared_mtx);
        cnd_broadcast(&thread_sync->cond_workers); // wake all workers to start next epoch
        wait_all_threads(thread_sync);
        mtx_unlock(&thread_sync->shared_mtx);
    }
    return 0;
}

void update_training_stats(TrainingStats *stats,
                           float *fitnesses,
                           genann *best_brain,
                           const genann *population,
                           Individual *elitists,
                           size_t *elitist_i)
{
    for (size_t pop_i = 0; pop_i < POP_SIZE; pop_i++)
    {
        float fit = fitnesses[pop_i];

        if (stats->min_fit > fit)
            stats->min_fit = fit;
        if (stats->max_fit < fit)
            stats->max_fit = fit;
        if (stats->alltime_max_fit < fit)
        {
            *elitist_i = (*elitist_i + 1) % ELITIST_AMOUNT;
            copy_weights(population + pop_i, elitists[*elitist_i].brain);
            stats->alltime_max_fit = fit;
            TraceLog(LOG_INFO, "set brain to %g fitness", fit);
            copy_weights(population + pop_i, best_brain);
        }
        stats->avg_fit += fit;
    }

    stats->avg_fit /= POP_SIZE;
    for (size_t i = 0; i < TREND_WINDOW - 1; i++)
    {
        stats->trend_window[i] = stats->trend_window[i + 1];
    }

    stats->trend_window[TREND_WINDOW - 1] = stats->avg_fit;
    stats->fit_trend = 0;
    for (size_t i = 0; i < TREND_WINDOW - 1; i++)
    {
        float diff = stats->trend_window[i + 1] - stats->trend_window[i];
        stats->fit_trend += diff;
    }
    stats->fit_trend /= TREND_WINDOW;
}

int creature_train(Creature *creature)
{
    RandomState *rng = random_make_seed();
    genann *population = init_population(rng, creature);
    genann *population_b_gen = init_population(rng, creature);
    float *fitnesses = malloc(sizeof fitnesses[0] * POP_SIZE);
    TrainingStats stats = make_training_stats();
    double crss_abort_chance;

    const int cores = n_cores();
    assert(cores > 0);

    // These are passed by pointer to individual threads for synchronization
    ThreadsSync thread_sync = {
        .thread_amount = cores,
        .finished_count = 0,
        .epoch = 0,
        .quit = false,
    };
    mtx_init(&thread_sync.shared_mtx, mtx_plain);
    cnd_init(&thread_sync.cond_workers);
    cnd_init(&thread_sync.cond_main);

    thrd_t *threads = malloc(sizeof threads[0] * cores);
    ThreadData *thread_data = make_threads_data(rng, population, fitnesses, creature, &thread_sync);

    Individual elitists[ELITIST_AMOUNT];
    for (size_t elt_i = 0; elt_i < ELITIST_AMOUNT; elt_i++)
    {
        elitists[elt_i].brain = genann_copy(creature->brain);
    }

    size_t elitist_i = 0;

    if (evaluate_threads(thread_data, threads, &thread_sync) != 0)
        return 1;

    update_training_stats(&stats, fitnesses, creature->brain, population, elitists, &elitist_i);

    for (int generation = 0; generation < EVOLUTION_GENERATIONS; generation++)
    {
        crss_abort_chance = crossover_abort_chance(generation);
        TraceLog(LOG_INFO, "%3d generation, fitness: [max: %+8.3f, avg: %+8.3f, min: %+8.3f, trend: %+8.3f], crossover_abort_chance: [%.3f]",
                 generation,
                 stats.max_fit,
                 stats.avg_fit,
                 stats.min_fit,
                 stats.fit_trend,
                 crss_abort_chance);
        stats.max_fit = -INFINITY;
        stats.min_fit = INFINITY;
        stats.avg_fit = 0;

        for (size_t elt_i = 0; elt_i < ELITIST_AMOUNT; elt_i++)
        {
            copy_weights(elitists[elt_i].brain, population_b_gen + elt_i);
        }
        for (size_t pop_i = ELITIST_AMOUNT; pop_i < POP_SIZE; pop_i++)
        {
            Individual ind_a = select(rng, population, fitnesses);
            Individual ind_b = select(rng, population, fitnesses);

            Individual *stronger;
            Individual *weaker;
            if (ind_a.fitness > ind_b.fitness)
            {
                stronger = &ind_a;
                weaker = &ind_b;
            }
            else
            {
                stronger = &ind_b;
                weaker = &ind_a;
            }

            crossover(rng, *stronger, *weaker, population_b_gen + pop_i);
            mutation(rng, population_b_gen + pop_i);
        }

        genann *temp;
        temp = population;
        population = population_b_gen;
        population_b_gen = temp;

        if (evaluate_threads(thread_data, threads, &thread_sync) != 0)
            return 1;
        update_training_stats(&stats, fitnesses, creature->brain, population, elitists, &elitist_i);
    }

    TraceLog(LOG_INFO, "All time best: %g", stats.alltime_max_fit);
    TraceLog(LOG_DEBUG, "Freeing training resources");

    free_population(population);
    free_population(population_b_gen);
    random_destroy(rng);
    destroy_threads(threads, thread_data, &thread_sync);

    return 0;
}