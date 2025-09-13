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

#define TREND_WINDOW 15 // How many generations to use to calculate the trend

#define POP_SIZE 90
#define EVOLUTION_GENERATIONS 300

#define EVALUATION_TESTS 6
#define MAX_EVALUATION_STEPS 60 * 15                                  // maximum evaluation steps in noneterminstic mode, actual amount otherwise
#define MIN_EVALUATION_STEPS 60 * 3                                   // minimum evaluation steps
#define EVALUATION_EARLY_TERMINATION_STEPS 10                         // After how many steps without movement to terminate
#define MIN_EVALUATION_PENALTY 0.2                                    // Multiplier to reduce from deduction (1 will reduce full value 0.5 half etc...)
#define MAX_EVALUATION_PENALTY 1                                      // Multiplier to reduce from deduction (1 will reduce full value 0.5 half etc...)
#define END_EVALUATION_PENALTY_GENERATION (EVOLUTION_GENERATIONS / 3) // The generation by which EVALUATION_PENALTY will reach it's maximum
#define EVALUATION_EARLY_TERMINATION_PENALTY 10                       // Points to deduce

#define ELITIST_AMOUNT 8

#define BASE_TOURNAMENT_SIZE POP_SIZE // Tournament size is calculated dynamically using the fitness trend. This values is used as a base value
#define CROSSOVER_BIAS 0.5            // How biased should crossover be towards the stronger parent

#define BASE_MUTATION_RATE 0.3  // Same as tournament size, this is dynamic. How many weights will change from the whole
#define MAX_MUTATION_AMOUNT 0.7 // How much each weight will be able to change

#define CROSSOVER_ABORT_MAX 0.5
#define CROSSOVER_ABORT_MIN 0

#endif // OPTIMIZER_RUN

// bigger then the amount of instructions
static_assert(MIN_EVALUATION_STEPS < MAX_EVALUATION_STEPS);
static_assert(EVALUATION_TESTS > 4);
static_assert(ELITIST_AMOUNT >= 0 && ELITIST_AMOUNT < POP_SIZE);
static_assert(CROSSOVER_BIAS >= 0 && CROSSOVER_BIAS <= 1);
static_assert(MAX_MUTATION_AMOUNT <= 1);
static_assert(BASE_TOURNAMENT_SIZE <= POP_SIZE);

int creature_train(Creature *creature);

#endif // TRAINER_HEADER
