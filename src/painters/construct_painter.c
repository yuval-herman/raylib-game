#include "construct_painter.h"
#include "construct.h"
#include "draw_manager.h"
#include "utils.h"
#include <stdlib.h>

struct Construct_Painter
{
    Construct *construct;
    int node_count;
    int joint_count;
    Draw_Circle **node_circles;
    Draw_Line **joint_lines;
    Construct_node* construct_nodes;
    Construct_joint* construct_joints;
};

Draw_Circle DEFAULT_NODE_SHAPE = {
            .pos = {0, 0},
            .radius = 0,
            .color = {255, 0, 0, 255}};
Draw_Line DEFAULT_JOINT_SHAPE = {
            .start = {0, 0},
            .end = {0, 0},
            .color = {0, 0, 0, 255},
            .width = 1};

Construct_Painter *construct_painter_make(Construct *construct)
{
    Construct_Painter *painter = malloc(sizeof *painter);

    painter->construct = construct;
    painter->node_count = construct_get_node_count(construct);
    painter->joint_count = construct_get_joint_count(construct);
    painter->construct_nodes = malloc(sizeof(Construct_node) * painter->node_count);
    painter->construct_joints = malloc(sizeof(Construct_joint) * painter->joint_count);

    // Create circles for nodes
    painter->node_circles = malloc(sizeof(Draw_Circle *) * painter->node_count);
    for (int i = 0; i < painter->node_count; i++)
    {
        painter->node_circles[i] = draw_register_circle(DEFAULT_NODE_SHAPE);
    }

    // Create lines for joints
    painter->joint_lines = malloc(sizeof(Draw_Line *) * painter->joint_count);
    for (int i = 0; i < painter->joint_count; i++)
    {
        painter->joint_lines[i] = draw_register_line(DEFAULT_JOINT_SHAPE);
    }

    log_debug("Allocated construct painter");
    return painter;
}

void construct_painter_update(Construct_Painter *painter)
{
    // Update nodes
    construct_get_nodes(painter->construct, painter->construct_nodes, painter->node_count);

    for (int i = 0; i < painter->node_count; i++)
    {
        painter->node_circles[i]->pos.x = painter->construct_nodes[i].pos.x;
        painter->node_circles[i]->pos.y = painter->construct_nodes[i].pos.y;
        painter->node_circles[i]->radius = painter->construct_nodes[i].radius;
    }

    // Update joints
    // construct_get_joints(painter->construct, painter->construct_joints, painter->joint_count);

    for (int i = 0; i < painter->joint_count; i++)
    {
        painter->joint_lines[i]->start = painter->node_circles[painter->construct_joints[i].node1_idx]->pos;
        painter->joint_lines[i]->end = painter->node_circles[painter->construct_joints[i].node2_idx]->pos;
    }
}

void construct_painter_destroy(Construct_Painter *painter)
{
    free(painter->node_circles);
    free(painter->joint_lines);
    free(painter->construct_nodes);
    free(painter->construct_joints);

    free(painter);
}
