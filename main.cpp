#include <SFML/Graphics.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <immintrin.h>

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
    for (int p_y = 0; p_y < HEIGHT; p_y++) {
        for (int p_x = 0; p_x < WIDTH / 8; p_x++) {
            __m256 rng8 = _mm256_set_ps(7.0f, 6.0f, 5.0f, 4.0f, 3.0f, 2.0f, 1.0f, 0.0f);
            __m256 height8 = _mm256_set1_ps((float)HEIGHT);
            __m256 zoom8 = _mm256_set1_ps(screen_state.zoom);

            __m256 p_x8 = _mm256_add_ps(_mm256_set1_ps((float)p_x*8), rng8);

            p_x8 = _mm256_div_ps(p_x8, height8);

            p_x8 = _mm256_add_ps(p_x8, _mm256_set1_ps(-((float)WIDTH / (float)HEIGHT) / 2.0f));

            p_x8 = _mm256_div_ps(p_x8, zoom8);

            __m256 cx8 = _mm256_add_ps(_mm256_set1_ps(screen_state.x_pos), p_x8);

            __m256 p_y8 = _mm256_set1_ps((float)p_y);

            p_y8 = _mm256_div_ps(p_y8, height8);

            p_y8 = _mm256_add_ps(p_y8, _mm256_set1_ps(-0.5f));

            p_y8 = _mm256_div_ps(p_y8, zoom8);

            p_x8 = _mm256_mul_ps(p_x8, _mm256_set1_ps(-1.0f));

            __m256 cy8 = _mm256_add_ps(_mm256_set1_ps(-screen_state.y_pos), p_y8);

            __m256i n8 = _mm256_setzero_si256();

            __m256 zx8 = _mm256_setzero_ps();
            __m256 zy8 = _mm256_setzero_ps();
            for (int i = 0; i < MAX_ITERATIONS; i++) {
                //(zx + izy)^2 = zx^2 - zy^2 + 2*zx*zy*

                __m256 zx8_2 = _mm256_mul_ps(zx8, zx8);
                __m256 zy8_2 = _mm256_mul_ps(zy8, zy8);

                __m256 new_zx8 = _mm256_add_ps(_mm256_sub_ps(zx8_2, zy8_2), cx8);

                __m256 zxzy8 = _mm256_mul_ps(zx8, zy8);
                __m256 new_zy8 = _mm256_add_ps(_mm256_add_ps(zxzy8, zxzy8), cy8);

                zx8 = new_zx8;
                zy8 = new_zy8;

                __m256 r8_2 = _mm256_add_ps(zx8_2, zy8_2);
                __m256 n4_8 = _mm256_set1_ps(4.0f);
                __m256i cmp8 = _mm256_castps_si256(_mm256_cmp_ps(n4_8, r8_2, _CMP_GT_OQ)); 

                n8 = _mm256_sub_epi32(n8, cmp8);

                if (_mm256_testz_si256(cmp8, cmp8)) {
                    break;
                }
            }
            
            __m256i r_div_255 = _mm256_set1_epi32(255);

            __m256i r8 = _mm256_and_si256(_mm256_mullo_epi32(n8, _mm256_set1_epi32(7)),  r_div_255);
            __m256i g8 = _mm256_and_si256(_mm256_mullo_epi32(n8, _mm256_set1_epi32(11)), r_div_255);
            __m256i b8 = _mm256_and_si256(_mm256_mullo_epi32(n8, _mm256_set1_epi32(17)), r_div_255);
            __m256i a8 = _mm256_set1_epi32(255);

            g8 = _mm256_slli_epi32(g8, 8);
            b8 = _mm256_slli_epi32(b8, 16);
            a8 = _mm256_slli_epi32(a8, 24);

            __m256i rgba8 = _mm256_or_si256(_mm256_or_si256(r8, g8), _mm256_or_si256(b8, a8));

            sfColor* start_pixel = pixels + p_y * WIDTH + p_x * 8;
            _mm256_storeu_si256((__m256i*)start_pixel, rgba8);
        }
    }

    return 0;
}