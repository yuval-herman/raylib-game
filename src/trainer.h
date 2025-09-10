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

// Numbers divisible by 18 (my laptop cores ¯\(ツ)/¯)
// 18, 36, 54, 72, 90, 108, 126, 144, 162, 180, 198

#ifndef OPTIMIZER_RUN

#define ENABLE_THREADS true

#define TREND_WINDOW 10 // How many generations to use to calculate the trend

#define POP_SIZE 144
#define EVOLUTION_GENERATIONS 300

#define EVALUATION_TESTS (8)
#define EVALUATION_STEPS (60 * 15)
#define EVALUATION_EARLY_TERMINATION_STEPS (60)  // After how many steps without movement to terminate
#define EVALUATION_PENALTY (0.5f)                // Multiplier to reduce from deduction (1 will reduce full value 0.5 half etc...)
#define EVALUATION_EARLY_TERMINATION_PENALTY (2) // Points to deduce

#define ELITIST_AMOUNT 10
static_assert(ELITIST_AMOUNT >= 0 && ELITIST_AMOUNT < POP_SIZE);

#define BASE_TOURNAMENT_SIZE (POP_SIZE) // Tournament size is calculated dynamically using the fitness trend. This values is used as a base value
#define CROSSOVER_BIAS 0.6              // How biased should crossover be towards the stronger parent
static_assert(CROSSOVER_BIAS >= 0 && CROSSOVER_BIAS <= 1);

#define BASE_MUTATION_RATE 0.005 // Same as tournament size, this is dynamic. How many weights will change from the whole
#define MAX_MUTATION_AMOUNT 0.05 // How much each weight will be able to change
static_assert(MAX_MUTATION_AMOUNT <= 1);

#define CROSSOVER_ABORT_MAX 0.5
#define CROSSOVER_ABORT_MIN 0

#endif // OPTIMIZER_RUN

int creature_train(Creature *creature);

#endif // TRAINER_HEADER
