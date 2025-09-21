#include "utils.h"
#include "draw_manager.h"

int main(void)
{
    init_window();

    while (!window_should_close())
    {
        draw();
    }

    deinit_window();

    return 0;
}
