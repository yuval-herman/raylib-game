#include "physics.h"
#include "draw_manager.h"
#include "construct.h"
#include "random.h"


int main(void)
{
    RandomState *rng = random_make();
    Construct *construct = construct_make(physics_make_world());
    construct_add_node(construct, 1, (b2Vec2){-1, 0});
    construct_add_node(construct, 1, (b2Vec2){1, 0});
    construct_add_joint(construct, 0, 1, true);
    construct_finalize(construct, rng);

    draw_register_circle((Draw_Circle){{0,0}, 10, {255, 0,0,255}});
    
    draw_window_make();

    while (!draw_window_should_close())
    {
        draw_draw();
        construct_update(construct, INST_NONE);
    }

    draw_window_destroy();

    random_destroy(rng);

    return 0;
}
