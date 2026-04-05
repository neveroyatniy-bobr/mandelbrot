#include <SFML/Graphics.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>

static const int  WIDTH = 800;
static const int HEIGHT = 600;
static const int    BPP =  32;

static const int MAX_ITERATIONS = 256;

static const float MOVE_SENS = 0.05;
static const float ZOOM_SENS = 1.05;

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

int main() {
    sfRenderWindow* window = sfRenderWindow_create((sfVideoMode){WIDTH, HEIGHT, BPP}, "Random pixels", sfClose, NULL);

    sfTexture* texture = sfTexture_create(WIDTH, HEIGHT);
    sfSprite*   sprite = sfSprite_create();
    sfSprite_setTexture(sprite, texture, sfTrue);

    sfColor* pixels = (sfColor*)calloc(WIDTH * HEIGHT, 4);

    sfClock* clock = sfClock_create();
    srand(time(NULL));

    float x_pos = 0, y_pos = 0;
    float zoom = 1;

    while (sfRenderWindow_isOpen(window)) {
        sfEvent event;
        while (sfRenderWindow_pollEvent(window, &event)) {
            if (event.type == sfEvtClosed) {
                sfRenderWindow_close(window);
            }
            if (event.type == sfEvtKeyPressed) {
                if (event.key.code == sfKeyEscape ) sfRenderWindow_close(window);
                if (event.key.code == sfKeyW      ) y_pos += MOVE_SENS / zoom;
                if (event.key.code == sfKeyS      ) y_pos -= MOVE_SENS / zoom;
                if (event.key.code == sfKeyA      ) x_pos -= MOVE_SENS / zoom;
                if (event.key.code == sfKeyD      ) x_pos += MOVE_SENS / zoom;
                if (event.key.code == sfKeyE      ) zoom *= ZOOM_SENS;
                if (event.key.code == sfKeyQ      ) zoom /= ZOOM_SENS;
            }
        }

        for (int p_i = 0; p_i < WIDTH * HEIGHT; p_i++) {
            sfColor* pixel = pixels + p_i;
            float cx = x_pos + ((float)(p_i%WIDTH) / (float)HEIGHT - 1.33f/2) / zoom       ;
            float cy = y_pos + ((float)(p_i/WIDTH) / (float)HEIGHT -  1.0f/2) / zoom * (-1);

            int n = GetN(cx, cy);
            pixel->r = n *  7 % 255;
            pixel->g = n * 11 % 255;
            pixel->b = n * 17 % 255;
            pixel->a = 255;
        }

        sfTexture_updateFromPixels(texture, (sfUint8*)pixels, WIDTH, HEIGHT, 0, 0);
        printf("%d\n", (int)(1 / sfTime_asSeconds(sfClock_getElapsedTime(clock))));
        sfClock_restart(clock);


        sfRenderWindow_drawSprite(window, sprite, NULL);
        sfRenderWindow_display(window);
    }
    
    return 0;
}