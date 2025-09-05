#include "creature.h"

#include <stdlib.h>

const float default_node_radius = 2.0f;
const float default_motor_speed = 1.0f;
const float max_motor_speed = -100.0f;
const float min_motor_speed = 100.0f;
const float default_motor_force = 200.0f;

b2BodyId make_node(b2Vec2 pos, float radius)
{
    b2BodyDef body_def = b2DefaultBodyDef();
    body_def.type = b2_dynamicBody;
    body_def.position = pos;
    body_def.motionLocks.angularZ = true;

    b2BodyId body_id = b2CreateBody(world_id, &body_def);

    b2ShapeDef shape_def = b2DefaultShapeDef();
    shape_def.material.friction = 1;
    shape_def.density = 1;

    b2Circle circle = {.center = b2Vec2_zero, .radius = radius || default_node_radius};
    b2CreateCircleShape(body_id, &shape_def, &circle);

    return body_id;
}

b2JointId connect_nodes(b2BodyId node1, b2BodyId node2, JointData *joint_data)
{
    b2DistanceJointDef joint_def = b2DefaultDistanceJointDef();

    joint_def.base.bodyIdA = node1;
    joint_def.base.bodyIdB = node2;
    joint_def.base.userData = joint_data;

    b2Vec2 anchorA = b2Body_GetWorldPoint(node1, b2Vec2_zero);
    b2Vec2 anchorB = b2Body_GetWorldPoint(node2, b2Vec2_zero);
    joint_def.minLength = default_node_radius;
    joint_def.maxLength = b2Distance(anchorA, anchorB);

    joint_def.enableSpring = true;
    joint_def.enableLimit = true;
    joint_def.enableMotor = true;

    joint_def.motorSpeed = joint_data->rest_motor_speed;
    joint_def.maxMotorForce = default_motor_force;

    return b2CreateDistanceJoint(world_id, &joint_def);
}

Creature creature_make()
{
    unsigned int node_amount = 3;
    b2BodyId *node_ids = malloc(sizeof node_ids[0] * node_amount);
    node_ids[0] = make_node((b2Vec2){.x = 5, .y = 15}, 0);
    node_ids[1] = make_node((b2Vec2){.x = 10, .y = 15}, 0);
    node_ids[2] = make_node((b2Vec2){.x = 7.5, .y = 10}, 0);

    unsigned int joint_amount = 3;
    b2JointId *joint_ids = malloc(sizeof joint_ids[0] * joint_amount);
    JointData *joints_data = malloc(sizeof joints_data[0] * joint_amount);
    joints_data[0] = (JointData){.rest_motor_speed = default_motor_speed};
    joint_ids[0] = connect_nodes(node_ids[0], node_ids[1], joints_data);
    joints_data[1] = (JointData){.rest_motor_speed = default_motor_speed};
    joint_ids[1] = connect_nodes(node_ids[1], node_ids[2], (joints_data + 1));
    joints_data[2] = (JointData){.rest_motor_speed = default_motor_speed};
    joint_ids[2] = connect_nodes(node_ids[2], node_ids[0], (joints_data + 2));

    return (Creature){
        .node_amount = node_amount,
        .node_ids = node_ids,

        .joint_amount = joint_amount,
        .joint_ids = joint_ids,
        .joints_data = joints_data};
}
// void creature_destroy()
// {
//     // TODO
// }
void creature_draw(Creature creature)
{
    // This is inefficient, I am checking bodies positions twice.
    // The reason is that I want joints to be drawn beneath nodes.
    // It could possibly be done more efficiently, but let's leave a TODO here in hopes of getting back to this,
    // as is traditionally done.
    for (size_t joint_i = 0; joint_i < creature.joint_amount; joint_i++)
    {
        Vector2 body_a_pos = b2rVec(b2Body_GetPosition(b2Joint_GetBodyA(creature.joint_ids[joint_i])));
        Vector2 body_b_pos = b2rVec(b2Body_GetPosition(b2Joint_GetBodyB(creature.joint_ids[joint_i])));
        DrawLineEx(body_a_pos, body_b_pos, 1, BLACK);
    }
    for (unsigned int body_i = 0; body_i < creature.node_amount; body_i++)
    {
        // Get body position and shape
        b2BodyId node_id = creature.node_ids[body_i];
        Vector2 pos = b2rVec(b2Body_GetPosition(node_id));

        b2ShapeId shape_id;
        b2Body_GetShapes(node_id, &shape_id, 1);
        b2Circle circle = b2Shape_GetCircle(shape_id);

        // Draw the node after the joints so it appears on top
        DrawCircleV(pos, circle.radius, RED);
    }
}
void creature_update(Creature *creature)
{
    if (IsKeyDown(KEY_RIGHT))
    {
        b2DistanceJoint_SetMotorSpeed(creature->joint_ids[0], min_motor_speed);
    }
    else if (IsKeyDown(KEY_LEFT))
    {
        b2DistanceJoint_SetMotorSpeed(creature->joint_ids[0], max_motor_speed);
    }
    else if (IsKeyReleased(KEY_RIGHT) || IsKeyReleased(KEY_LEFT))
    {
        for (size_t i = 0; i < creature->joint_amount; i++)
        {
            JointData *joint_data = b2Joint_GetUserData(creature->joint_ids[i]);
            b2DistanceJoint_SetMotorSpeed(creature->joint_ids[i], joint_data->rest_motor_speed);
        }
    }
}