#include "box2d.h"
#include "physics.h"
#include "draw_manager.h"
#include "construct.h"
#include "random.h"
#include "utils.h"
#include "painters/construct_painter.h"

int main(void)
{
    RandomState *rng = random_make();

    b2WorldId world_id = physics_make_world();
    physics_make_ground(world_id);
    draw_register_rectangle((Draw_Rectangle){
        .pos = (Draw_Vector2){-GROUND_EXTENT, -GROUND_HEIGHT},
        .width = GROUND_EXTENT * 2,
        .height = GROUND_HEIGHT,
        .color = {0, 0, 0, 255},
    });

    Construct *construct = construct_make(world_id);
    construct_add_node(construct, (Construct_node){1, (b2Vec2){10, 10}});
    construct_add_node(construct, (Construct_node){1, (b2Vec2){1, 0}});
    construct_add_joint(construct, (Construct_joint){0, 1, true});
    construct_finalize(construct, rng);

    Construct_Painter *painter = construct_painter_make(construct);

    draw_window_make();

    while (!draw_window_should_close())
    {
        draw_draw();
        b2World_Step(world_id, TIME_STEP, SUB_STEP_COUNT);
        construct_update(construct, INST_NONE);
        construct_painter_update(painter);
    }

    draw_window_destroy();

    construct_painter_destroy(painter);
    construct_destroy(construct);
    random_destroy(rng);

    return 0;
}
