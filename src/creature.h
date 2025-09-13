#ifndef CREATURE_HEADER
#define CREATURE_HEADER

#include <stddef.h>

#include "physics.h"
#include "random.h"
#include "genann.h"

// TODO: should assert on these in code
#define MAX_NODES (10)
#define MAX_JOINTS (MAX_NODES * 3)

typedef struct JointData
{
    unsigned int node_a_idx;
    unsigned int node_b_idx;
    float rest_motor_speed;
} JointData;

typedef struct Creature
{
    genann *brain;
    // Used in creature_think
    double *brain_inputs;

    b2WorldId world_id;

    b2BodyId *node_ids;
    unsigned int node_amount;
    b2Vec2 *original_node_positions;
    float node_radius;

    b2JointId *joint_ids;
    JointData *joints_data;
    unsigned int joint_amount;
} Creature;

typedef enum CreatureInstruction
{
    INST_NONE,
    INST_LEFT,
    INST_RIGHT,
    INST_UP,
} CreatureInstruction;

Creature creature_make(RandomState *rng, b2WorldId world_id,
                       b2Vec2 *node_positions,
                       unsigned int node_amount,
                       JointData *joints,
                       unsigned int joint_amount);
void creature_reset(Creature *creature);
// Creature is emptied and can be safely reused after a call to this function
void creature_destroy(Creature *creature);
void creature_draw(Creature creature);
void creature_update(Creature *creature);
void creature_think(Creature *creature, CreatureInstruction);
b2Vec2 creature_get_center(const Creature *creature);
void creature_rotate(Creature *creature, b2Vec2 origin, float rad);

#endif // CREATURE_HEADER