#include <stdlib.h>
#include <string.h>

#include "trainer.h"

// Get average node position
b2Vec2 get_avg_position(Creature *creature)
{
    b2Vec2 avg = {0};
    for (unsigned int body_i = 0; body_i < creature->node_amount; body_i++)
    {
        b2Vec2 pos = b2Body_GetPosition(creature->node_ids[body_i]);
        avg.x += pos.x;
        avg.y += pos.y;
    }
    avg.x /= creature->node_amount;
    avg.y /= creature->node_amount;
    return avg;
}

// Train creature using genetic algorithm

// Initialization
// Selection
// Crossover
// Mutation

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

double evaluate(Creature *creature)
{
    // TODO: I'm ignoring UP for now since it's a bit complicated
    CreatureInstruction inst_arr[] = {INST_NONE, INST_LEFT, INST_RIGHT};

    double fitness = 0;
    CreatureInstruction inst;

    creature_reset(creature);
    for (int test = 0; test < EVALUATION_TESTS; test++)
    {
        inst = inst_arr[GetRandomValue(0, ARRAY_COUNT(inst_arr) - 1)];
        b2Vec2 start_pos = get_avg_position(creature);
        double last_pos = start_pos.x;
        for (int i = 0; i < EVALUATION_STEPS; i++)
        {
            creature_think(creature, inst);
            b2World_Step(world_id, TIME_STEP, SUB_STEP_COUNT);
            b2Vec2 pos = get_avg_position(creature);

            // TODO: actually check if the creature is grounded
            if (inst != INST_NONE && (fabs(last_pos - pos.x) < FLT_EPSILON || pos.y < 1.1))
                break;
            last_pos = pos.x;
        }

        double distance = fabs(last_pos - start_pos.x);
        if (inst == INST_NONE)
        {
            fitness -= distance * 2;
        }
        else
        {
            bool moved_right = last_pos > start_pos.x;
            bool should_move_right = inst == INST_RIGHT;
            if (moved_right == should_move_right)
                fitness += distance;
            else
                fitness /= 2;

            // TraceLog(LOG_INFO, "Test %d: Inst=%d, Distance=%.2f, Moved%s, Should%s, Fitness=%.2f",
            //          test, inst, distance,
            //          moved_right ? "Right" : "Left",
            //          should_move_right ? "Right" : "Left",
            //          test_fitness);
        }
    }
    return fitness;
}

genann *select(genann *population, double *fitnesses)
{
    size_t index_max = GetRandomValue(0, POP_SIZE - 1);
    for (int i = 0; i < TOURNAMENT_SIZE; i++)
    {
        size_t index_check = GetRandomValue(0, POP_SIZE - 1);
        if (fitnesses[index_max] < fitnesses[index_check])
            index_max = index_check;
    }

    return population + index_max;
}

void crossover(const genann *ind_a, const genann *ind_b, genann *child)
{
    assert(ind_a->total_weights == ind_b->total_weights);

    for (int i = 0; i < ind_a->total_weights; ++i)
    {
        child->weight[i] = GENANN_RANDOM() < 0.5 ? ind_a->weight[i] : ind_b->weight[i];
    }
}

void mutation(genann *ind, double mutation_rate)
{
    for (int i = 0; i < ind->total_weights; ++i)
    {
        if (GENANN_RANDOM() < mutation_rate)
        {
            ind->weight[i] += (GENANN_RANDOM() - 0.5) * MUTATION_AMOUNT;
            ind->weight[i] = b2ClampFloat(ind->weight[i], -0.5, 0.5);
        }
    }
}

void creature_train(Creature *creature)
{

    genann *population = init_population(creature);
    genann *population_b_gen = init_population(creature);
    double *fitnesses = malloc(sizeof fitnesses[0] * POP_SIZE);
    double max_fit = -INFINITY;

    genann_free(creature->brain);

    for (int generation = 0; generation < EVOLUTION_GENERATIONS; generation++)
    {
        TraceLog(LOG_INFO, "training %d generation", generation);
        for (size_t pop_i = 0; pop_i < POP_SIZE; pop_i++)
        {
            creature->brain = population + pop_i;
            fitnesses[pop_i] = evaluate(creature);
            if (max_fit < fitnesses[pop_i])
            {
                max_fit = fitnesses[pop_i];
                TraceLog(LOG_INFO, "gen %d, max fit increased %g", generation, max_fit);
            }
        }
        for (size_t pop_i = 0; pop_i < POP_SIZE; pop_i++)
        {
            genann *ind_a = select(population, fitnesses);
            genann *ind_b = select(population, fitnesses);

            crossover(ind_a, ind_b, population_b_gen + pop_i);
            mutation(population_b_gen + pop_i, MUTATION_RATE);
        }

        genann *temp;
        temp = population;
        population = population_b_gen;
        population_b_gen = temp;
    };

    // Set the brain to the best brain
    max_fit = DBL_MIN;
    size_t max_index = 0;
    for (size_t pop_i = 0; pop_i < POP_SIZE; pop_i++)
    {
        creature->brain = population + pop_i;
        fitnesses[pop_i] = evaluate(creature);
        if (fitnesses[pop_i] > max_fit)
        {
            max_fit = fitnesses[pop_i];
            max_index = pop_i;
        }
    }
    creature->brain = genann_copy(population + max_index);
    creature_reset(creature);
    free_population(population);
    free_population(population_b_gen);
}