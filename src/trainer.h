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

#define POP_SIZE 180
#define EVOLUTION_GENERATIONS 10

#define EVALUATION_TESTS (10)
#define EVALUATION_STEPS (60 * 20)
#define EVALUATION_PENALTY (0.35f)

#define ELITIST_AMOUNT 5
static_assert(ELITIST_AMOUNT >= 0 && ELITIST_AMOUNT < POP_SIZE);

#define TOURNAMENT_SIZE (POP_SIZE / 10)
#define CROSSOVER_BIAS 0.5 // How biased should crossover be towards the stronger parent
static_assert(CROSSOVER_BIAS >= 0 && CROSSOVER_BIAS <= 1);

#define CROSSOVER_ABORT_MAX 0.5
#define CROSSOVER_ABORT_MIN 0

#define MUTATION_RATE 0.05      // How many weights will change from the whole
#define MAX_MUTATION_AMOUNT 0.1 // How much each weight will be able to change
static_assert(MAX_MUTATION_AMOUNT <= 1);

// #define DETERMINISTIC_TRAINING

int creature_train(Creature *creature);

#endif // TRAINER_HEADER