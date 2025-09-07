#include <stdlib.h>
#include <string.h>

#include "trainer.h"

#define POP_SIZE 100

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

genann **init_population(Creature *creature)
{
    genann **population = malloc(sizeof(genann *) * POP_SIZE);
    for (size_t i = 0; i < POP_SIZE; i++)
    {
        population[i] = genann_copy(creature->brain);
        genann_randomize(population[i]);
    }
    return population;
}
void free_population(genann **population)
{
    for (size_t i = 0; i < POP_SIZE; i++)
    {
        genann_free(population[i]);
    }
    free(population);
}

double evaluate(Creature *creature)
{
    creature_reset(creature);
    double last_pos = get_avg_position(creature).x;
    for (size_t i = 0; i < 60 * 15; i++)
    {
        creature_think(creature, GO_RIGHT);
        b2World_Step(world_id, TIME_STEP, SUB_STEP_COUNT);
        double pos = get_avg_position(creature).x;
        if (fabs(last_pos - pos) < FLT_EPSILON)
            break;
        last_pos = pos;
    }
    return last_pos;
}

genann *select(genann **population, double *fitnesses)
{
    size_t index_max = GetRandomValue(0, POP_SIZE - 1);
    for (int i = 0; i < 10; i++)
    {
        size_t index_check = GetRandomValue(0, POP_SIZE - 1);
        if (fitnesses[index_max] < fitnesses[index_check])
            index_max = index_check;
    }

    return population[index_max];
}

void crossover(const genann ind_a, const genann ind_b, genann *child)
{
    assert(ind_a.total_weights == ind_b.total_weights);

    for (int i = 0; i < ind_a.total_weights; ++i)
    {
        child->weight[i] = GENANN_RANDOM() < 0.5 ? ind_a.weight[i] : ind_b.weight[i];
    }
}

void mutation(genann *ind, double mutation_rate)
{
    for (int i = 0; i < ind->total_weights; ++i)
    {
        if (GENANN_RANDOM() < mutation_rate)
            ind->weight[i] = GENANN_RANDOM() - 0.5;
    }
}

void creature_train(Creature *creature)
{

    genann **population = init_population(creature);
    genann **population_b_gen = init_population(creature);
    double *fitnesses = malloc(sizeof fitnesses[0] * POP_SIZE);
    double max_fit = DBL_MIN;

    genann_free(creature->brain);

    for (int gens = 0; gens < 100; gens++)
    {
        for (size_t pop_i = 0; pop_i < POP_SIZE; pop_i++)
        {
            creature->brain = population[pop_i];
            fitnesses[pop_i] = evaluate(creature);
            if (max_fit < fitnesses[pop_i])
            {
                max_fit = fitnesses[pop_i];
                TraceLog(LOG_INFO, "max fit increased %g", max_fit);
            }
        }
        for (size_t pop_i = 0; pop_i < POP_SIZE; pop_i++)
        {
            genann *ind_a = select(population, fitnesses);
            genann *ind_b = select(population, fitnesses);

            crossover(*ind_a, *ind_b, population_b_gen[pop_i]);
            mutation(population_b_gen[pop_i], 0.025);
        }

        genann **temp;
        temp = population;
        population = population_b_gen;
        population_b_gen = temp;
    };

    // Set the brain to the best brain
    max_fit = DBL_MIN;
    size_t max_index = 0;
    for (size_t pop_i = 0; pop_i < POP_SIZE; pop_i++)
    {
        creature->brain = population[pop_i];
        fitnesses[pop_i] = evaluate(creature);
        if (fitnesses[pop_i] > max_fit)
        {
            max_fit = fitnesses[pop_i];
            max_index = pop_i;
        }
    }
    creature->brain = genann_copy(population[max_index]);
    creature_reset(creature);
    free_population(population);
    free_population(population_b_gen);
}