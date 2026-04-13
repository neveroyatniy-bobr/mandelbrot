#include "native.hpp"

#include <SFML/Graphics.h>

#include "prop.hpp"
#include "screen_state.hpp"

int NativeUpdatePixels(ScreenState screen_state, sfColor* pixels) {
    for (int p_i = 0; p_i < WIDTH * HEIGHT; p_i++) {
        sfColor* pixel = pixels + p_i;
        float cx = screen_state.x_pos + ((float)(p_i%WIDTH) / (float)HEIGHT - 1.33f/2) / screen_state.zoom       ;
        float cy = screen_state.y_pos + ((float)(p_i/WIDTH) / (float)HEIGHT -  1.0f/2) / screen_state.zoom * (-1);

        int n = GetN(cx, cy);
        pixel->r = n *  7 % 255;
        pixel->g = n * 11 % 255;
        pixel->b = n * 17 % 255;
        pixel->a = 255;
    }

    return 0;
}

int GetN(float cx, float cy) {
    float zx = 0, zy = 0;
    for (int i = 0; i < MAX_ITERATIONS; i++) {
        //(zx + izy)^2 = zx^2 - zy^2 + 2*zx*zy*
        float new_zx = zx*zx - zy*zy + cx;
        float new_zy = 2*zx*zy + cy;
        zx = new_zx;
        zy = new_zy;

        if (zx*zx + zy*zy >= 4.0f) {
            return i;
        }
    }

    return 255;
}