#include <stdlib.h>
#include <string.h>

#include "creature.h"
#include "raylib.h"
#include "raymath.h"

const float default_node_radius = 1.0f;
const float default_motor_speed = 1.0f;
const float max_motor_speed = 100.0f;
const float min_motor_speed = -100.0f;
const float default_motor_force = 200.0f;

b2Vec2 creature_get_center(const Creature *creature)
{
    b2Vec2 center = b2Vec2_zero;
    for (unsigned int body_i = 0; body_i < creature->node_amount; body_i++)
    {
        b2Vec2 pos = b2Body_GetPosition(creature->node_ids[body_i]);
        center.x += pos.x;
        center.y += pos.y;
    }
    center.x /= creature->node_amount;
    center.y /= creature->node_amount;
    return center;
}

// Gets all node positions in an interleaved array [x,y,x,y,...]
// Positions are relative to creature center
// Returns the center postion
b2Vec2 get_node_rel_positions(Creature *creature, double *node_positions)
{
    b2Vec2 center = b2Vec2_zero;
    for (unsigned int body_i = 0; body_i < creature->node_amount; body_i++)
    {
        b2Vec2 pos = b2Body_GetPosition(creature->node_ids[body_i]);
        center.x += pos.x;
        center.y += pos.y;
        node_positions[body_i * 2] = pos.x;
        node_positions[body_i * 2 + 1] = pos.y;
    }

    // Calculate average center position
    center.x /= creature->node_amount;
    center.y /= creature->node_amount;
    for (unsigned int body_i = 0; body_i < creature->node_amount; body_i++)
    {
        node_positions[body_i * 2] -= center.x;
        node_positions[body_i * 2 + 1] -= center.y;
    }
    return center;
}

// Gets all node linear velocities in an interleaved array [x,y,x,y,...]
void get_node_velocities(Creature *creature, double *node_velocities)
{
    for (unsigned int body_i = 0; body_i < creature->node_amount; body_i++)
    {
        b2Vec2 vel = b2Body_GetLinearVelocity(creature->node_ids[body_i]);
        node_velocities[body_i * 2] = vel.x;
        node_velocities[body_i * 2 + 1] = vel.y;
    }
}

b2BodyId make_node(b2WorldId world_id, b2Vec2 pos, float radius)
{
    assert(radius > 0);
    b2BodyDef body_def = b2DefaultBodyDef();
    body_def.type = b2_dynamicBody;
    body_def.position = pos;
    body_def.motionLocks.angularZ = true;

    b2BodyId body_id = b2CreateBody(world_id, &body_def);

    b2ShapeDef shape_def = b2DefaultShapeDef();
    shape_def.material.friction = 1;
    shape_def.density = 1;

    b2Circle circle = {.center = b2Vec2_zero, .radius = radius};
    b2CreateCircleShape(body_id, &shape_def, &circle);

    return body_id;
}

b2JointId connect_nodes(b2WorldId world_id, b2BodyId node1, b2BodyId node2, JointData *joint_data)
{
    b2DistanceJointDef joint_def = b2DefaultDistanceJointDef();

    joint_def.base.bodyIdA = node1;
    joint_def.base.bodyIdB = node2;
    joint_def.base.userData = joint_data;

    b2Vec2 anchorA = b2Body_GetWorldPoint(node1, b2Vec2_zero);
    b2Vec2 anchorB = b2Body_GetWorldPoint(node2, b2Vec2_zero);

    const float dist = b2Distance(anchorA, anchorB);

    joint_def.minLength = dist * 0.5;
    joint_def.length = dist;
    joint_def.maxLength = dist * 1.5;

    joint_def.enableLimit = true;
    joint_def.enableSpring = joint_data->is_muscle;
    joint_def.enableMotor = joint_data->is_muscle;

    joint_def.motorSpeed = joint_data->rest_motor_speed;
    joint_def.maxMotorForce = default_motor_force;

    return b2CreateDistanceJoint(world_id, &joint_def);
}

Creature creature_make(RandomState *rng, b2WorldId world_id, b2Vec2 *node_positions, unsigned int node_amount, JointData *joints, unsigned int joint_amount)
{
    b2Vec2 *owned_node_positions = malloc(sizeof node_positions[0] * node_amount);
    memcpy(owned_node_positions, node_positions, sizeof node_positions[0] * node_amount);
    // Nodes
    b2BodyId *node_ids = malloc(sizeof node_ids[0] * node_amount);
    for (size_t node_i = 0; node_i < node_amount; node_i++)
    {
        node_ids[node_i] = make_node(world_id, node_positions[node_i], default_node_radius);
    }

    // Joints
    b2JointId *joint_ids = malloc(sizeof joint_ids[0] * joint_amount);
    JointData *joints_data = malloc(sizeof joints_data[0] * joint_amount);
    for (size_t joint_i = 0; joint_i < joint_amount; joint_i++)
    {
        joints_data[joint_i] = joints[joint_i];
        if (joints_data[joint_i].rest_motor_speed == 0)
            joints_data[joint_i].rest_motor_speed = default_motor_speed;

        joint_ids[joint_i] = connect_nodes(world_id, node_ids[joints[joint_i].node_a_idx],
                                           node_ids[joints[joint_i].node_b_idx], joints_data + joint_i);
    }

    // Brain
    // Inputs:
    int inputs =
        // node positions [x, y]
        node_amount * 2
        // node velocities [x, y]
        + node_amount * 2
        // center height
        + 1
        // instruction flags
        + 3;
    // Outputs:
    // motor speeds for joints
    double *brain_inputs = malloc(sizeof(double) * inputs);
    int hidden_layers = 2;
    int hidden_nodes = inputs * 3;
    int outputs = joint_amount;
    genann *ann = genann_init(rng, inputs,
                              hidden_layers,
                              hidden_nodes,
                              outputs);

    return (Creature){
        .node_amount = node_amount,
        .node_ids = node_ids,
        .original_node_positions = owned_node_positions,
        .node_radius = default_node_radius,

        .joint_amount = joint_amount,
        .joint_ids = joint_ids,
        .joints_data = joints_data,

        .world_id = world_id,

        .brain = ann,
        .brain_inputs = brain_inputs,
    };
}

// This DOES NOT destroy box2d bodies and shapes
void creature_destroy(Creature *creature)
{
    genann_free(creature->brain);
    free(creature->brain_inputs);
    free(creature->joint_ids);
    free(creature->joints_data);
    free(creature->original_node_positions);
    free(creature->node_ids);
}

void creature_reset_motors(Creature *creature)
{
    for (size_t i = 0; i < creature->joint_amount; i++)
    {
        JointData *joint_data = b2Joint_GetUserData(creature->joint_ids[i]);
        b2DistanceJoint_SetMotorSpeed(creature->joint_ids[i], joint_data->rest_motor_speed);
    }
}

// Reset all nodes and joints, does not reset the brain
void creature_reset(Creature *creature)
{
    // Nodes
    for (size_t node_i = 0; node_i < creature->node_amount; node_i++)
    {
        b2Body_SetTransform(creature->node_ids[node_i], creature->original_node_positions[node_i], b2Rot_identity);
        b2Body_SetLinearVelocity(creature->node_ids[node_i], b2Vec2_zero);
        b2Body_SetAngularVelocity(creature->node_ids[node_i], 0);
    }

    // Joints
    creature_reset_motors(creature);
}

// Rotate the creature by a radian around a given point
void creature_rotate(Creature *creature, b2Vec2 origin, float rad)
{
    float cos_o = cosf(rad);
    float sin_o = sinf(rad);

    for (size_t node_i = 0; node_i < creature->node_amount; node_i++)
    {
        b2Vec2 pos = b2Sub(b2Body_GetPosition(creature->node_ids[node_i]), origin);
        // Use intermediate variables so can update pos.x and not overwrite the original x postion
        float x = pos.x, y = pos.y;
        pos.x = x * cos_o - y * sin_o;
        pos.y = x * sin_o + y * cos_o;
        b2Body_SetTransform(creature->node_ids[node_i], b2Add(pos, origin), b2Rot_identity);
    }
}

void creature_draw(Creature creature)
{
    // TODO:
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
    Vector2 center = {0};
    float top = INFINITY;
    for (unsigned int body_i = 0; body_i < creature.node_amount; body_i++)
    {
        // Get body position and shape
        b2BodyId node_id = creature.node_ids[body_i];
        Vector2 pos = b2rVec(b2Body_GetPosition(node_id));
        center.x += pos.x;
        center.y += pos.y;
        if (pos.y < top)
            top = pos.y;

        // Draw the node after the joints so it appears on top
        DrawCircleV(pos, creature.node_radius, RED);
    }
    const Font font = GetFontDefault();
    center.x /= creature.node_amount;
    center.y /= creature.node_amount;
    // reverse y because of previous b2rVec use
    const char *text = TextFormat("%.2f,%.2f", center.x, -center.y);
    const float font_size = 3;
    const float font_spacing = 1;
    Vector2 text_measure = MeasureTextEx(font, text, font_size, font_spacing);
    center.x -= text_measure.x / 2;
    center.y = top - font_size - 1;
    DrawTextEx(font, text, center, font_size, font_spacing, BLACK);
}

void creature_think(Creature *creature, CreatureInstruction inst)
{
    // first are nod position, then instruction flags
    double *inputs = creature->brain_inputs;
    b2Vec2 center = get_node_rel_positions(creature, inputs);
    inputs += creature->node_amount * 2;
    get_node_velocities(creature, inputs);
    inputs += creature->node_amount * 2;
    inputs[0] = center.y;

    inputs[1] = inst == INST_LEFT ? 1 : -1;
    inputs[2] = inst == INST_RIGHT ? 1 : -1;
    inputs[3] = inst == INST_NONE ? 1 : -1;

    const double *motor_speeds = genann_run(creature->brain, creature->brain_inputs);
    for (size_t i = 0; i < creature->joint_amount; i++)
    {
        float motor_speed = Remap((float)motor_speeds[i], 0, 1, min_motor_speed, max_motor_speed);
        b2DistanceJoint_SetMotorSpeed(creature->joint_ids[i], motor_speed);
    }
}

void creature_update(Creature *creature, RandomState *rng)
{
    if (IsKeyPressed(KEY_Q))
    {
        creature_rotate(creature, creature_get_center(creature), random_float(rng) * 2 * B2_PI);
    }
    if (IsKeyPressed(KEY_R))
    {
        creature_reset(creature);
    }

    if (IsKeyDown(KEY_RIGHT))
    {
        creature_think(creature, INST_RIGHT);
    }
    else if (IsKeyDown(KEY_LEFT))
    {
        creature_think(creature, INST_LEFT);
    }
    else if (IsKeyDown(KEY_SPACE))
    {
        creature_reset_motors(creature);
    }
    else
    {
        creature_think(creature, INST_NONE);
    }
}