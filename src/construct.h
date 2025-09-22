#if !defined(CONSTRUCT_H)
#define CONSTRUCT_H

#define genann_act genann_act_threshold
#include "genann.h"

typedef enum ConstructInstruction
{
    INST_NONE,
    INST_LEFT,
    INST_RIGHT,
    INST_UP,
} ConstructInstruction;

typedef struct Construct Construct;

Construct *construct_make();
Construct *construct_destroy();

void construct_update();
void construct_input_update();

#endif // CONSTRUCT_H