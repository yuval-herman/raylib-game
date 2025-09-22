#include "construct.h"
#include "box2d.h"

typedef struct JointsData
{
    b2JointId *joint_ids;
    unsigned int *node_a_idx;
    unsigned int *node_b_idx;
    unsigned int joint_amount;
    bool *is_muscle;
} JointsData;

typedef struct NodesData
{
    b2BodyId *node_ids;
    b2Vec2 *original_node_position;
    unsigned int node_amount;
} NodesData;

typedef struct Construct
{
    genann *brain;
    // Used to reduce allocations
    double *brain_inputs;

    b2WorldId world_id;

    NodesData nodes_data;
    JointsData joints_data;
} Construct;

b2BodyId make_node(b2WorldId world_id, b2Vec2 pos)
{
    b2BodyDef body_def = b2DefaultBodyDef();
    body_def.type = b2_dynamicBody;
    body_def.position = pos;
    body_def.motionLocks.angularZ = true;

    b2BodyId body_id = b2CreateBody(world_id, &body_def);

    b2ShapeDef shape_def = b2DefaultShapeDef();
    shape_def.material.friction = 1;
    shape_def.density = 1;

    b2Circle circle = {.center = b2Vec2_zero, .radius = 1};
    b2CreateCircleShape(body_id, &shape_def, &circle);

    return body_id;
}

b2JointId make_joint(b2WorldId world_id, b2BodyId node1, b2BodyId node2,
                     JointsData joints_data, unsigned int joint_idx)
{
    b2DistanceJointDef joint_def = b2DefaultDistanceJointDef();

    joint_def.base.bodyIdA = node1;
    joint_def.base.bodyIdB = node2;

    b2Vec2 anchorA = b2Body_GetWorldPoint(node1, b2Vec2_zero);
    b2Vec2 anchorB = b2Body_GetWorldPoint(node2, b2Vec2_zero);

    const float dist = b2Distance(anchorA, anchorB);

    joint_def.minLength = dist * 0.5;
    joint_def.length = dist;
    joint_def.maxLength = dist * 1.5;

    joint_def.enableLimit = true;
    joint_def.enableSpring = joints_data.is_muscle[joint_idx];
    joint_def.enableMotor = joints_data.is_muscle[joint_idx];

    joint_def.motorSpeed = 1;
    joint_def.maxMotorForce = 200.0f;

    return b2CreateDistanceJoint(world_id, &joint_def);
}

Construct *construct_make(RandomState *rng, b2WorldId world_id, b2Vec2 *node_positions, unsigned int node_amount, JointData *joints, unsigned int joint_amount)
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

Construct *construct_destroy();

void construct_update();
void construct_input_update();
