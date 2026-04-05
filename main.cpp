#include <SFML/Graphics.h>
#include <stdlib.h>
#include <time.h>

static const int  WIDTH = 800;
static const int HEIGHT = 600;
static const int    BPP =  32;

int main() {
    sfRenderWindow* window = sfRenderWindow_create((sfVideoMode){WIDTH, HEIGHT, BPP}, "Random pixels", sfClose, NULL);

    sfTexture* texture = sfTexture_create(WIDTH, HEIGHT);
    sfSprite*   sprite = sfSprite_create();
    sfSprite_setTexture(sprite, texture, sfTrue);

    sfColor* pixels = (sfColor*)calloc(WIDTH * HEIGHT, 4);

    sfClock* clock = sfClock_create();
    srand(time(NULL));

    while (sfRenderWindow_isOpen(window)) {
        sfEvent event;
        while (sfRenderWindow_pollEvent(window, &event)) {
            if (event.type == sfEvtClosed) {
                sfRenderWindow_close(window);
            }
        }
        
        if (sfTime_asSeconds(sfClock_getElapsedTime(clock)) >= 1.0f) {
            for (int p_i = 0; p_i < WIDTH * HEIGHT; p_i++) {
                sfColor* pixel = pixels + p_i;
                pixel->r = rand() % 256;
                pixel->g = rand() % 256;
                pixel->b = rand() % 256;
                pixel->a = 255;
            }

            sfTexture_updateFromPixels(texture, (sfUint8*)pixels, WIDTH, HEIGHT, 0, 0);
            sfClock_restart(clock);
        }

        sfRenderWindow_drawSprite(window, sprite, NULL);
        sfRenderWindow_display(window);
    }
    
    return 0;
}