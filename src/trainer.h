#ifndef TRAINER_HEADER
#define TRAINER_HEADER

#include "creature.h"

#define POP_SIZE 100
#define TOURNAMENT_SIZE (POP_SIZE / 10)
#define EVOLUTION_GENERATIONS 50

#define EVALUATION_STEPS (60 * 10)
#define EVALUATION_TESTS (6)

#define MUTATION_RATE 0.025
#define MUTATION_AMOUNT 0.5
static_assert(MUTATION_AMOUNT <= 1);

void creature_train(Creature *creature);

#endif // TRAINER_HEADER