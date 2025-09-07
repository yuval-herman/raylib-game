#ifndef CREATURE_HEADER
#define CREATURE_HEADER

#include <stddef.h>

#include "physics.h"
#include "genann.h"

// TODO: should assert on these in code
#define MAX_NODES (10)
#define MAX_JOINTS (MAX_NODES * 3)

typedef struct JointData
{
    float rest_motor_speed;
} JointData;

typedef struct Creature
{
    genann *brain;

    b2BodyId *node_ids;
    unsigned int node_amount;

    b2JointId *joint_ids;
    JointData *joints_data;
    unsigned int joint_amount;
} Creature;

typedef enum CreatureInstruction
{
    GO_LEFT,
    GO_RIGHT,
    GO_UP,
} CreatureInstruction;

Creature creature_make();
void creature_reset(Creature *creature);
// Creature is emptied and can be safely reused after a call to this function
void creature_destroy(Creature *creature);
void creature_draw(Creature creature);
void creature_update(Creature *creature);
void creature_think(Creature *creature, CreatureInstruction);

#endif // CREATURE_HEADER