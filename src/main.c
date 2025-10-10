#include "box2d.h"
#include "physics.h"
#include "draw_manager.h"
#include "construct.h"
#include "random.h"
#include "utils.h"

int main(void)
{
    RandomState *rng = random_make();

    b2WorldId world_id = physics_make_world();
    physics_make_ground(world_id);
    draw_register_rectangle((Draw_Rectangle){
                            .pos = (Draw_Vector2){-GROUND_EXTENT, -GROUND_HEIGHT},
                            .width = GROUND_EXTENT*2,
                            .height = GROUND_HEIGHT,
                            .color = {0,0,0,255},
                        });
    
    Construct *construct = construct_make(world_id);
    construct_add_node(construct, (Construct_node){1, (b2Vec2){10, 10}});
    construct_add_node(construct, (Construct_node){1, (b2Vec2){1, 0}});
    construct_add_joint(construct, (Construct_joint){0, 1, true});
    construct_finalize(construct, rng);

    Construct_node nodes[2];
    construct_get_nodes(construct, nodes, 2);

    Draw_Circle* node1 = draw_register_circle((Draw_Circle){0});
    Draw_Circle* node2 = draw_register_circle((Draw_Circle){0});
    Draw_Line* joint = draw_register_line((Draw_Line){0});
    joint->color = (Draw_Color){0,0,0,255};
    joint->width = 1;
    
    draw_window_make();

    while (!draw_window_should_close())
    {
        draw_draw();
        b2World_Step(world_id, TIME_STEP, SUB_STEP_COUNT);
        construct_update(construct, INST_NONE);
        construct_get_nodes(construct, nodes, 2);
        node1->pos.x = nodes[0].pos.x;
        node1->pos.y = nodes[0].pos.y;
        node1->radius = nodes[0].radius;
        node1->color = (Draw_Color){255,0,0,255};
        node2->pos.x = nodes[1].pos.x;
        node2->pos.y = nodes[1].pos.y;
        node2->radius = nodes[1].radius;
        node2->color = (Draw_Color){255,0,0,255};
        joint->start = node1->pos;
        joint->end = node2->pos;
    }

    draw_window_destroy();

    construct_destroy(construct);
    random_destroy(rng);

    return 0;
}
