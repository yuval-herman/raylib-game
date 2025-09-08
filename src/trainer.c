#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "trainer.h"

typedef struct Individual
{
    genann *brain;
    float fitness;
} Individual;

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

genann *init_population(Creature *creature)
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

        genann_randomize(((genann *)population) + i);
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

float evaluate(Creature *creature)
{
    // TODO: I'm ignoring UP for now since it's a bit complicated
    CreatureInstruction inst_arr[] = {INST_NONE, INST_LEFT, INST_RIGHT};

    float fitness = 0;
    CreatureInstruction inst;

    creature_reset(creature);
    for (int test = 0; test < EVALUATION_TESTS; test++)
    {
#ifdef DETERMINISTIC_TRAINING
        inst = inst_arr[test % ARRAY_COUNT(inst_arr)];
#else
        inst = inst_arr[GetRandomValue(0, ARRAY_COUNT(inst_arr) - 1)];
#endif
        b2Vec2 start_pos = get_avg_position(creature);
        float last_pos = start_pos.x;
        for (int i = 0; i < EVALUATION_STEPS; i++)
        {
            creature_think(creature, inst);
            b2World_Step(world_id, TIME_STEP, SUB_STEP_COUNT);
            b2Vec2 pos = get_avg_position(creature);

            if (inst != INST_NONE && (fabsf(last_pos - pos.x) < FLT_EPSILON))
                break;
            last_pos = pos.x;
        }

        float distance = fabsf(last_pos - start_pos.x);
        if (inst == INST_NONE)
        {
            fitness -= distance * EVALUATION_PENALTY;
        }
        else
        {
            bool moved_right = last_pos > start_pos.x;
            bool should_move_right = inst == INST_RIGHT;
            if (moved_right == should_move_right)
                fitness += distance;
            else
                fitness -= distance * EVALUATION_PENALTY;
        }
    }
    return fitness;
}

Individual select(genann *population, float *fitnesses)
{
    size_t index_max = GetRandomValue(0, POP_SIZE - 1);
    for (int i = 0; i < TOURNAMENT_SIZE; i++)
    {
        size_t index_check = GetRandomValue(0, POP_SIZE - 1);
        if (fitnesses[index_max] < fitnesses[index_check])
            index_max = index_check;
    }
    return (Individual){.brain = population + index_max, .fitness = fitnesses[index_max]};
}

void crossover(const Individual stronger, const Individual weaker, genann *child)
{
    assert(stronger.fitness >= weaker.fitness);
    assert(stronger.brain->total_weights == weaker.brain->total_weights);
    for (int i = 0; i < stronger.brain->total_weights; ++i)
    {
        child->weight[i] = GENANN_RANDOM() < CROSSOVER_BIAS ? stronger.brain->weight[i] : weaker.brain->weight[i];
    }
}

void mutation(genann *ind)
{
    for (int i = 0; i < ind->total_weights; ++i)
    {
        if (GENANN_RANDOM() < MUTATION_RATE)
        {
            ind->weight[i] += (GENANN_RANDOM() - 0.5) * MAX_MUTATION_AMOUNT;
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
    const double target_rate = 0.05;
    const double min_rate = 0.001;
    return b2MaxFloat(min_rate, ((double)generation * target_rate) / EVOLUTION_GENERATIONS);
}

void creature_train(Creature *creature)
{
#ifdef DETERMINISTIC_TRAINING
    /* Disable warm starting while training so the solver does not reuse cached impulse
     * accumulators from previous simulations. This reduces cross-evaluation
     * nondeterminism when we reset body transforms between runs. */
    b2World_EnableWarmStarting(world_id, false);
#endif
    genann *population = init_population(creature);
    genann *population_b_gen = init_population(creature);
    float *fitnesses = malloc(sizeof fitnesses[0] * POP_SIZE);

    genann *best_brain = creature->brain;

    double crss_abort_chance;

    Individual elitists[ELITIST_AMOUNT];
    for (size_t elt_i = 0; elt_i < ELITIST_AMOUNT; elt_i++)
    {
        elitists[elt_i].brain = genann_copy(best_brain);
    }

    size_t elitist_i = 0;

    float alltime_max_fit = -INFINITY;
    float max_fit = -INFINITY, min_fit = INFINITY, avg_fit = 0, fit;
    for (size_t pop_i = 0; pop_i < POP_SIZE; pop_i++)
    {
        creature->brain = population + pop_i;
        fit = evaluate(creature);
        fitnesses[pop_i] = fit;

        if (min_fit > fit)
            min_fit = fit;
        if (max_fit < fit)
            max_fit = fit;
        if (alltime_max_fit < fit)
        {
            elitist_i = (elitist_i + 1) % ELITIST_AMOUNT;
            copy_weights(population + pop_i, elitists[elitist_i].brain);
            elitists[elitist_i].fitness = fit;
            alltime_max_fit = fit;
            copy_weights(creature->brain, best_brain);
        }
        avg_fit += fit;
    }
    avg_fit /= POP_SIZE;

    for (int generation = 0; generation < EVOLUTION_GENERATIONS; generation++)
    {
        crss_abort_chance = crossover_abort_chance(generation);
        TraceLog(LOG_INFO, "%d generation, fitness: [max: %+8.3f, avg: %+8.3f, min: %+8.3f], crossover_abort_chance: [%.3f]",
                 generation,
                 max_fit,
                 avg_fit,
                 min_fit,
                 crss_abort_chance);
        max_fit = -INFINITY;
        min_fit = INFINITY;
        avg_fit = 0;

        for (size_t elt_i = 0; elt_i < ELITIST_AMOUNT; elt_i++)
        {
            copy_weights(elitists[elt_i].brain, population_b_gen + elt_i);
            fitnesses[elt_i] = elitists[elt_i].fitness;
        }
        for (size_t pop_i = ELITIST_AMOUNT; pop_i < POP_SIZE; pop_i++)
        {
            Individual ind_a = select(population, fitnesses);
            Individual ind_b = select(population, fitnesses);

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

            crossover(*stronger, *weaker, population_b_gen + pop_i);
            mutation(population_b_gen + pop_i);
            creature->brain = population_b_gen + pop_i;
            float child_fit = evaluate(creature);
            if (child_fit < stronger->fitness && GENANN_RANDOM() < crss_abort_chance)
            {
                copy_weights(stronger->brain, population_b_gen + pop_i);
                fit = stronger->fitness;
            }
            else
            {
                fit = child_fit;
            }
            if (min_fit > fit)
                min_fit = fit;
            if (max_fit < fit)
                max_fit = fit;
            if (alltime_max_fit < fit)
            {
                elitist_i = (elitist_i + 1) % ELITIST_AMOUNT;
                copy_weights(population + pop_i, elitists[elitist_i].brain);
                elitists[elitist_i].fitness = fit;
                alltime_max_fit = fit;
                copy_weights(creature->brain, best_brain);
            }
            avg_fit += fit;
            fitnesses[pop_i] = fit;
        }
        avg_fit /= POP_SIZE;

        genann *temp;
        temp = population;
        population = population_b_gen;
        population_b_gen = temp;
    };

    size_t max_index = 0;
    for (size_t pop_i = 0; pop_i < POP_SIZE; pop_i++)
    {
        creature->brain = population + pop_i;
        fitnesses[pop_i] = evaluate(creature);
        if (fitnesses[pop_i] > alltime_max_fit)
        {
            alltime_max_fit = fitnesses[pop_i];
            max_index = pop_i;
        }
    }

    copy_weights(population + max_index, best_brain);
    creature->brain = best_brain;

    TraceLog(LOG_INFO, "set brain to %g fitness", alltime_max_fit);
#ifdef DETERMINISTIC_TRAINING
    // Re-enable warm starting
    b2World_EnableWarmStarting(world_id, true);
#endif
    for (size_t elt_i = 0; elt_i < ELITIST_AMOUNT; elt_i++)
    {
        genann_free(elitists[elt_i].brain);
    }
    creature_reset(creature);
    free_population(population);
    free_population(population_b_gen);
}