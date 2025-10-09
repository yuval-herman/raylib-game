#include "math_functions.h"
#if !defined(CONSTRUCT_H)
#define CONSTRUCT_H

#define genann_act genann_act_threshold
#include "genann.h"
#include "physics.h"
#include "random.h"

#define MAX_NODES 256
#define MAX_JOINTS 1024

typedef enum ConstructInstruction
{
    INST_NONE,
    INST_LEFT,
    INST_RIGHT,
    INST_UP,
} ConstructInstruction;

typedef struct Construct_node {
    float radius;
    b2Vec2 pos;
} Construct_node;

typedef struct Construct Construct;

// Creates an empty construct. The construct should be built incrementally using build functions like add_node.
// An empty construct cannot be used until finalized!
Construct *construct_make(b2WorldId world_id);
// Finalizes a construct.
void construct_finalize(Construct *c, RandomState *rng);
void construct_destroy(Construct *c);

// Add a node to creature and returns its index. Returns -1 if there are max amount of nodes already.
int construct_add_node(Construct *c, Construct_node node);
// Add a joint to creature and returns its index. Returns -1 if there are max amount of joints already.
int construct_add_joint(Construct *c, int node1_idx, int node2_idx, bool is_muscle);

int construct_get_node_amount(Construct *c);
void construct_get_nodes(Construct *c, Construct_node* nodes, int max_nodes);

void construct_update(Construct *construct, ConstructInstruction inst);

#endif // CONSTRUCT_H
