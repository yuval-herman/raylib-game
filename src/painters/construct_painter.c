#include "construct_painter.h"
#include "construct.h"
#include "draw_manager.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>

struct Construct_Painter
{
    Construct *construct;
    int node_count;
    int joint_count;
    size_t* draw_node_handles;
    size_t* draw_joint_handles;
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
    painter->draw_node_handles = malloc(sizeof(size_t) * painter->node_count);
    for (int i = 0; i < painter->node_count; i++)
    {
        painter->draw_node_handles[i] = draw_register_circle(DEFAULT_NODE_SHAPE);
    }

    // Create lines for joints
    painter->draw_joint_handles = malloc(sizeof(size_t) * painter->joint_count);
    for (int i = 0; i < painter->joint_count; i++)
    {
        painter->draw_joint_handles[i] = draw_register_line(DEFAULT_JOINT_SHAPE);
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
        Draw_Circle* circle = draw_get_circle(painter->draw_node_handles[i]);
        circle->pos.x = painter->construct_nodes[i].pos.x;
        circle->pos.y = painter->construct_nodes[i].pos.y;
        circle->radius = painter->construct_nodes[i].radius;
    }

    // Update joints
    construct_get_joints(painter->construct, painter->construct_joints, painter->joint_count);

    for (int i = 0; i < painter->joint_count; i++)
    {
        Draw_Line* line = draw_get_line(painter->draw_joint_handles[i]);
        line->start.x = painter->construct_nodes[painter->construct_joints[i].node1_idx].pos.x;
        line->start.y = painter->construct_nodes[painter->construct_joints[i].node1_idx].pos.y;
        line->end.x = painter->construct_nodes[painter->construct_joints[i].node2_idx].pos.x;
        line->end.y = painter->construct_nodes[painter->construct_joints[i].node2_idx].pos.y;
    }
}

void construct_painter_destroy(Construct_Painter *painter)
{
    free(painter->draw_node_handles);
    free(painter->draw_joint_handles);
    free(painter->construct_nodes);
    free(painter->construct_joints);

    free(painter);
}
