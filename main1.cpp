#include <SFML/Graphics.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>

#include "timer.hpp"

#define TIME

static const int  WIDTH = 800;
static const int HEIGHT = 600;
static const int    BPP =  32;

static const int MAX_ITERATIONS = 256;

static const float MOVE_SENS = 1;
static const float ZOOM_SENS = 1.05;

struct ScreenState {
    float x_pos, y_pos;
    float zoom;
};

int HadleEvents(ScreenState* screen_state, sfRenderWindow* window, float dt);

int UpdatePixels(ScreenState screen_state, sfColor* pixels);

int GetN(float cx, float cy);

int main() {
    sfRenderWindow* window = sfRenderWindow_create((sfVideoMode){WIDTH, HEIGHT, BPP}, "Random pixels", sfClose, NULL);

    sfTexture* texture = sfTexture_create(WIDTH, HEIGHT);
    sfSprite*   sprite = sfSprite_create();
    sfSprite_setTexture(sprite, texture, sfTrue);

    sfColor* pixels = (sfColor*)calloc(WIDTH * HEIGHT, 4);

    sfClock* clock = sfClock_create();

    ScreenState screen_state = {
        .x_pos = 0,
        .y_pos = 0,
        .zoom  = 1
    };

    #ifdef TIME
        const int MEASUREMENT_CNT = 100;
        const char* calc_times_file_name = "calc_times.csv";

        Timer timer = {};
        size_t calc_times[MEASUREMENT_CNT] = {};
        int mes_i = 0;

        for(mes_i = 0; mes_i < MEASUREMENT_CNT; mes_i++) {
            StartTimer(&timer);

            UpdatePixels(screen_state, pixels);
            __asm__ volatile("" : : "g" (pixels) : "memory");

            EndTimer(&timer);
            calc_times[mes_i] = GetTime(&timer);
        }

        FILE* calc_times_file = fopen(calc_times_file_name, "w");
        if (calc_times_file == NULL) {
            fprintf(stderr, "Не удалось сиздать файл с измерениями");
            return 1;
        }

        for(mes_i = 0; mes_i < MEASUREMENT_CNT; mes_i++) {
            fprintf(calc_times_file, "%ld\n", calc_times[mes_i]);
        }

        fclose(calc_times_file);
    #else
        while (sfRenderWindow_isOpen(window)) {
            float dt = sfTime_asSeconds(sfClock_getElapsedTime(clock));
            // printf("%d\n", (int)(1 / dt));
            sfClock_restart(clock);

            HadleEvents(&screen_state, window, dt);

            UpdatePixels(screen_state, pixels);
            
            sfTexture_updateFromPixels(texture, (sfUint8*)pixels, WIDTH, HEIGHT, 0, 0);

            sfRenderWindow_drawSprite(window, sprite, NULL);
            sfRenderWindow_display(window);
        }
    #endif

    return 0;
}

int HadleEvents(ScreenState* screen_state, sfRenderWindow* window, float dt) {
    sfEvent event;
    while (sfRenderWindow_pollEvent(window, &event)) {
        if (event.type == sfEvtClosed) {
            sfRenderWindow_close(window);
        }
        if (event.type == sfEvtKeyPressed) {
            if (event.key.code == sfKeyEscape ) sfRenderWindow_close(window);
            if (event.key.code == sfKeyD      ) screen_state->x_pos += MOVE_SENS * dt / screen_state->zoom;
            if (event.key.code == sfKeyW      ) screen_state->y_pos += MOVE_SENS * dt / screen_state->zoom;
            if (event.key.code == sfKeyS      ) screen_state->y_pos -= MOVE_SENS * dt / screen_state->zoom;
            if (event.key.code == sfKeyA      ) screen_state->x_pos -= MOVE_SENS * dt / screen_state->zoom;
            if (event.key.code == sfKeyE      ) screen_state->zoom *= ZOOM_SENS;
            if (event.key.code == sfKeyQ      ) screen_state->zoom /= ZOOM_SENS;
        }
    }

    return 0;
}

int UpdatePixels(ScreenState screen_state, sfColor* pixels) {
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