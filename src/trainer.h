#ifndef TRAINER_HEADER
#define TRAINER_HEADER

#include "creature.h"

/*
This are the parameters for the GA evolution algorithm.
They are mostly self-explanatory, here is the gist of it though:
For faster simulation, reduce POP_SIZE, EVOLUTION_GENERATIONS, EVALUATION_STEPS,
and EVALUATION_TESTS. The trade of is faster simulation for worse results.
You can try and reduce population size and evaluation accuracy, but increase generations count,
and test for effects.

For faster convergence you can increase TOURNAMENT_SIZE and decrease mutation, but this will also
lead to faster stalling.
You can also shift the CROSSOVER_BIAS towards 1 to select stronger genes, but again reduce diversity.
*/

#define POP_SIZE 100
#define EVOLUTION_GENERATIONS 50

#define EVALUATION_STEPS (60 * 10)
#define EVALUATION_TESTS (6)

#define TOURNAMENT_SIZE (POP_SIZE / 20)
#define CROSSOVER_BIAS 0.5 // How biased should crossover be towards the stronger parent
static_assert(CROSSOVER_BIAS >= 0 && CROSSOVER_BIAS <= 1);
#define CROSSOVER_ABORT_RATE 0.2 // The chances for picking a parent instead of a child if the child turned out worse then the parent

#define MUTATION_RATE 0.02            // How many weights will change from the whole
#define MAX_MUTATION_AMOUNT 0.0000001 // How much each weight will be able to change
static_assert(MAX_MUTATION_AMOUNT <= 1);

void creature_train(Creature *creature);

#endif // TRAINER_HEADER