#include "utils.h"
#include "draw_manager.h"
#include "construct.h"

int main(void)
{
    draw_window_make();

    while (!draw_window_should_close())
    {
        draw_draw();
    }

    draw_window_destroy();

    return 0;
}
