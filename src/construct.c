#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "utils.h"
#include "construct.h"
#include "raymath.h"

const float max_motor_speed = 100.0f;
const float min_motor_speed = -100.0f;

typedef struct Construct
{
    b2WorldId world_id;

    genann *brain;
    // Used to reduce allocations
    double *brain_inputs;

    b2BodyId *node_ids;
    int node_count;
    b2JointId *joint_ids;
    int joint_count;
} Construct;

/* ---------- Helpers ---------- */

static inline bool valid_construct(const Construct *c)
{
    return c != NULL;
}

b2Vec2 construct_get_center(Construct *construct)
{
    b2Vec2 center = b2Vec2_zero;
    assert(construct && construct->node_count > 0);

    for (int body_i = 0; body_i < construct->node_count; body_i++)
    {
        b2Vec2 pos = b2Body_GetPosition(construct->node_ids[body_i]);
        center.x += pos.x;
        center.y += pos.y;
    }
    center.x /= construct->node_count;
    center.y /= construct->node_count;
    return center;
}

/* Gets all node positions in an interleaved array [x,y,x,y,...]
 * Positions are relative to construct center
 * node_positions must point to at least node_count*2 doubles.
 * Returns the center position (world coords).
 */
b2Vec2 get_node_rel_positions(Construct *construct, double *node_positions)
{
    b2Vec2 center = b2Vec2_zero;
    assert(construct && construct->node_count > 0);

    for (int body_i = 0; body_i < construct->node_count; body_i++)
    {
        b2Vec2 pos = b2Body_GetPosition(construct->node_ids[body_i]);
        center.x += pos.x;
        center.y += pos.y;
        node_positions[body_i * 2] = pos.x;
        node_positions[body_i * 2 + 1] = pos.y;
    }

    // Calculate average center position and subtract to make positions relative.
    center.x /= construct->node_count;
    center.y /= construct->node_count;
    for (int body_i = 0; body_i < construct->node_count; body_i++)
    {
        node_positions[body_i * 2] -= center.x;
        node_positions[body_i * 2 + 1] -= center.y;
    }
    return center;
}

/* Gets all node linear velocities in an interleaved array [x,y,x,y,...]
 * node_velocities must point to at least node_count*2 doubles.
 */
void get_node_velocities(Construct *construct, double *node_velocities)
{
    assert(construct && construct->node_count > 0);

    for (int body_i = 0; body_i < construct->node_count; body_i++)
    {
        b2Vec2 vel = b2Body_GetLinearVelocity(construct->node_ids[body_i]);
        node_velocities[body_i * 2] = vel.x;
        node_velocities[body_i * 2 + 1] = vel.y;
    }
}

/* Gets all joint lengths into joint_lengths array (length joint_count). */
void get_joint_lengths(Construct *construct, double *joint_lengths)
{
    assert(construct && construct->joint_count > 0);

    for (int joint_i = 0; joint_i < construct->joint_count; joint_i++)
    {
        b2BodyId node_a = b2Joint_GetBodyA(construct->joint_ids[joint_i]);
        b2BodyId node_b = b2Joint_GetBodyB(construct->joint_ids[joint_i]);
        b2Vec2 anchor_a = b2Body_GetWorldPoint(node_a, b2Vec2_zero);
        b2Vec2 anchor_b = b2Body_GetWorldPoint(node_b, b2Vec2_zero);

        joint_lengths[joint_i] = b2Distance(anchor_a, anchor_b);
    }
}

bool construct_is_finalized(Construct *c)
{
    assert(c);
    return c->brain != NULL;
}

/* If a construct is finalized, make it un-finalized. */
void construct_make_unfinished(Construct *c)
{
    assert(c);
    if (!construct_is_finalized(c))
        return;
    genann_free(c->brain);
    c->brain = NULL;
    free(c->brain_inputs);
    c->brain_inputs = NULL;
    log_debug("un-finalized construct");
}

/* ---------- Lifecycle ---------- */

Construct *construct_make(b2WorldId world_id)
{
    Construct *construct = calloc(1, sizeof *construct);
    log_debug("Allocated new construct");
    construct->world_id = world_id;
    return construct;
}

/* Finalize: (re)create brain and brain_inputs. Requires at least one node and joint. */
void construct_finalize(Construct *c, RandomState *rng)
{
    assert(c && rng);
    assert(c->node_count > 0 && c->joint_count > 0);

    construct_make_unfinished(c);

    // Compute input/output sizes
    int inputs =
        // node positions [x, y]
        c->node_count * 2
        // node velocities [x, y]
        + c->node_count * 2
        // joints lengths
        + c->joint_count
        // center height
        + 1
        // instruction flags
        + 3;
    int hidden_layers = 2;
    int hidden_nodes = inputs * 3;
    int outputs = c->joint_count;

    c->brain_inputs = malloc(sizeof(double) * inputs);
    c->brain = genann_init(rng, inputs,
                           hidden_layers,
                           hidden_nodes,
                           outputs);
}

/* Destroys construct and also destroys physics bodies and joints in world. Safe to call with NULL members. */
void construct_destroy(Construct *c)
{
    assert(c);

    /* Destroy joints from physics world first */
    if (c->joint_ids)
    {
        for (int i = 0; i < c->joint_count; i++)
        {
            b2DestroyJoint(c->joint_ids[i], false);
        }
    }

    /* Destroy bodies */
    if (c->node_ids)
    {
        for (int i = 0; i < c->node_count; i++)
        {
            b2DestroyBody(c->node_ids[i]);
        }
    }

    genann_free(c->brain);
    c->brain = NULL;
    free(c->brain_inputs);
    c->brain_inputs = NULL;
    free(c->joint_ids);
    c->joint_ids = NULL;
    free(c->node_ids);
    c->node_ids = NULL;
    free(c);
    c = NULL;
}

/* ---------- Mutators ---------- */

/* Add a node and return its index or -1 on failure or max nodes reached. */
int construct_add_node(Construct *c, float radius, b2Vec2 pos)
{
    assert(c);
    if (c->node_count >= MAX_NODES)
    {
        log_msg(LOG_WARN, "tried adding node after reaching MAX_NODES (%d)", MAX_NODES);
        return -1;
    }
    assert(radius > 0);

    b2BodyDef body_def = b2DefaultBodyDef();
    body_def.type = b2_dynamicBody;
    body_def.position = pos;
    body_def.motionLocks.angularZ = true;

    b2BodyId body_id = b2CreateBody(c->world_id, &body_def);

    b2ShapeDef shape_def = b2DefaultShapeDef();
    shape_def.material.friction = 1;
    shape_def.density = 1;

    b2Circle circle = {.center = b2Vec2_zero, .radius = radius};
    b2CreateCircleShape(body_id, &shape_def, &circle);

    c->node_ids = realloc(c->node_ids, sizeof c->node_ids[0] * ++c->node_count);
    c->node_ids[c->node_count - 1] = body_id;

    log_debug("allocated new node");
    construct_make_unfinished(c);
    return c->node_count - 1;
}

/* Add a distance joint between node indices. Return joint index or -1 on failure. */
int construct_add_joint(Construct *c, int node1_idx, int node2_idx, bool is_muscle)
{
    assert(c);

    if (c->joint_count >= MAX_JOINTS)
    {
        log_msg(LOG_WARN, "tried adding joint after reaching MAX_JOINTS (%d)", MAX_JOINTS);
        return -1;
    }

    assert(node1_idx >= 0 && node2_idx >= 0);
    assert(node1_idx < c->node_count && node2_idx < c->node_count);

    b2DistanceJointDef joint_def = b2DefaultDistanceJointDef();

    b2BodyId node1 = c->node_ids[node1_idx];
    b2BodyId node2 = c->node_ids[node2_idx];

    joint_def.base.bodyIdA = node1;
    joint_def.base.bodyIdB = node2;

    b2Vec2 anchorA = b2Body_GetWorldPoint(node1, b2Vec2_zero);
    b2Vec2 anchorB = b2Body_GetWorldPoint(node2, b2Vec2_zero);

    const float dist = b2Distance(anchorA, anchorB);

    joint_def.minLength = dist * 0.5f;
    joint_def.length = dist;
    joint_def.maxLength = dist * 1.5f;

    joint_def.enableLimit = true;
    joint_def.enableSpring = is_muscle;
    joint_def.enableMotor = is_muscle;
    joint_def.motorSpeed = 1.0f;
    joint_def.maxMotorForce = 200.0f;

    c->joint_ids = realloc(c->joint_ids, sizeof c->joint_ids[0] * ++c->joint_count);
    c->joint_ids[c->joint_count - 1] = b2CreateDistanceJoint(c->world_id, &joint_def);

    log_debug("allocated new joint");
    construct_make_unfinished(c);
    return c->joint_count - 1;
}

/* Update: fills brain inputs and runs brain. Safe-guards added. */
void construct_update(Construct *construct, ConstructInstruction inst)
{
    assert(construct);
    assert(construct_is_finalized(construct));
    assert(construct->node_count > 0 && construct->joint_count > 0);

    double *inputs = construct->brain_inputs;
    /* layout must match finalize: node_pos(2*N), node_vel(2*N), joint_len(M), center(1), inst_flags(3) */
    b2Vec2 center = get_node_rel_positions(construct, inputs);
    inputs += construct->node_count * 2;
    get_node_velocities(construct, inputs);
    inputs += construct->node_count * 2;
    get_joint_lengths(construct, inputs);
    inputs += construct->joint_count;
    inputs[0] = center.y;
    inputs[1] = (inst == INST_LEFT) ? 1.0 : -1.0;
    inputs[2] = (inst == INST_RIGHT) ? 1.0 : -1.0;
    inputs[3] = (inst == INST_NONE) ? 1.0 : -1.0;

    const double *motor_speeds = genann_run(construct->brain, construct->brain_inputs);

    for (int i = 0; i < construct->joint_count; i++)
    {
        float motor_speed = Remap((float)motor_speeds[i], 0.0f, 1.0f, min_motor_speed, max_motor_speed);
        b2DistanceJoint_SetMotorSpeed(construct->joint_ids[i], motor_speed);
    }
}
