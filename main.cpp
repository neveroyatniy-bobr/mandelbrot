#include <SFML/Graphics.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>

#include "prop.hpp"
#include "intrinsics.hpp"
#include "screen_state.hpp"
#include "cmd_args_handler.hpp"

int HadleEvents(ScreenState* screen_state, sfRenderWindow* window, float dt);

int main(int argc, char** argv) {
    int (*UpdatePixels)(ScreenState screen_state, sfColor* pixels) = IntrinsicsUpdatePixels;
    HandleCmdArgs(argc, argv, &UpdatePixels);

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