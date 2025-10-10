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
    construct_add_joint(construct, 0, 1, true);
    construct_finalize(construct, rng);

    Construct_node nodes[2];
    construct_get_nodes(construct, nodes, 2);

    Draw_Circle* circle = draw_register_circle((Draw_Circle){0});
    
    draw_window_make();

    while (!draw_window_should_close())
    {
        draw_draw();
        b2World_Step(world_id, TIME_STEP, SUB_STEP_COUNT);
        construct_update(construct, INST_NONE);
        construct_get_nodes(construct, nodes, 2);
        circle->pos.x = nodes[0].pos.x;
        circle->pos.y = nodes[0].pos.y;
        circle->radius = nodes[0].radius;
        circle->color = (Draw_Color){255,0,0,255};
    }

    draw_window_destroy();

    construct_destroy(construct);
    random_destroy(rng);

    return 0;
}
