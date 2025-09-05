#ifndef CREATURE_HEADER
#define CREATURE_HEADER

#include <stddef.h>
#include "physics.h"

// TODO: should assert on these in code
#define MAX_NODES (10)
#define MAX_JOINTS (MAX_NODES * 3)

typedef struct JointData
{
    float rest_motor_speed;
} JointData;

typedef struct Creature
{
    b2BodyId *node_ids;
    unsigned int node_amount;

    b2JointId *joint_ids;
    JointData *joints_data;
    unsigned int joint_amount;
} Creature;

Creature creature_make();
void creature_destroy();
void creature_draw(Creature creature);
void creature_update(Creature *creature);

#endif // CREATURE_HEADER