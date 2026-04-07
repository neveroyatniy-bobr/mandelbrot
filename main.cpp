#include <SFML/Graphics.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <immintrin.h>

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

int float8_add(float a[8], float b[8]);
int float8_sub(float a[8], float b[8]);
int float8_div(float a[8], float b[8]);
int float8_mul(float a[8], float b[8]);
int float8_fil(float a[8], float b);
int float8_rng(float a[8]);
int float8_cpy(float a[8], float b[8]);
int float8_cmp(float a[8], float b[8], int cmp[8]);

int is_cmp_zero(int cmp[8]);
int int8_add(int a[8], int b[8]);

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

    while (sfRenderWindow_isOpen(window)) {
        float dt = sfTime_asSeconds(sfClock_getElapsedTime(clock));
        printf("%d\n", (int)(1 / dt));
        sfClock_restart(clock);

        HadleEvents(&screen_state, window, dt);

        UpdatePixels(screen_state, pixels);

        sfTexture_updateFromPixels(texture, (sfUint8*)pixels, WIDTH, HEIGHT, 0, 0);

        sfRenderWindow_drawSprite(window, sprite, NULL);
        sfRenderWindow_display(window);
    }
    
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
}

int UpdatePixels(ScreenState screen_state, sfColor* pixels) {
    for (int p_y = 0; p_y < HEIGHT; p_y++) {
        for (int p_x = 0; p_x < WIDTH / 8; p_x++) {
            float rng8[8] = {};
            float8_rng(rng8);

            float height8[8] = {};
            float8_fil(height8, (float)HEIGHT);

            float zoom8[8] = {};
            float8_fil(zoom8, screen_state.zoom);

            float cx8[8] = {};
            float8_fil(cx8, screen_state.x_pos);

            float p_x8[8] = {};
            float8_fil(p_x8, (float)p_x*8);
            float8_add(p_x8, rng8);

            float8_div(p_x8, height8);

            float dp_x8[8] = {};
            float8_fil(dp_x8, -((float)WIDTH / (float)HEIGHT) / 2.0f);
            float8_add(p_x8, dp_x8);

            float8_div(p_x8, zoom8);

            float8_add(cx8, p_x8);

            float cy8[8] = {};
            float8_fil(cy8, -screen_state.y_pos);

            float p_y8[8] = {};
            float8_fil(p_y8, (float)p_y);

            float8_div(p_y8, height8);

            float dp_y8[8] = {};
            float8_fil(dp_y8, -0.5f);
            float8_add(p_y8, dp_y8);

            float8_div(p_y8, zoom8);

            float minus8[8] = {};
            float8_fil(minus8, -1.0f);
            float8_mul(p_x8, minus8);

            float8_add(cy8, p_y8);

            int n8[8] = {0, 0, 0, 0, 0, 0, 0, 0};

            float zx8[8] = {};
            float zy8[8] = {};
            float8_fil(zx8, 0.0f);
            float8_fil(zy8, 0.0f);
            for (int i = 0; i < MAX_ITERATIONS; i++) {
                //(zx + izy)^2 = zx^2 - zy^2 + 2*zx*zy*

                float zx8_2[8] = {};
                float8_cpy(zx8_2, zx8);
                float8_mul(zx8_2, zx8);
                float zy8_2[8] = {};
                float8_cpy(zy8_2, zy8);
                float8_mul(zy8_2, zy8);

                float new_zx8[8] = {}; //zx2 - zy2 + cx
                float8_cpy(new_zx8, zx8_2);
                float8_sub(new_zx8, zy8_2);
                float8_add(new_zx8, cx8);

                float new_zy8[8] = {}; //2*zx*zy + cy;
                float8_fil(new_zy8, 2.0f);
                float8_mul(new_zy8, zx8);
                float8_mul(new_zy8, zy8);
                float8_add(new_zy8, cy8);

                float8_cpy(zx8, new_zx8);
                float8_cpy(zy8, new_zy8);

                float r8_2[8] = {};
                float8_cpy(r8_2, zx8_2);
                float8_add(r8_2, zy8_2);

                float n4_8[8] = {};
                float8_fil(n4_8, 4.0f);

                int cmp8[8] = {};
                float8_cmp(n4_8, r8_2, cmp8);

                int8_add(n8, cmp8);

                if (is_cmp_zero(cmp8)) {
                    break;
                }
            }
            
            sfColor* start_pixel = pixels + p_y * WIDTH + p_x * 8;
            for (int p_i = 0; p_i < 8; p_i++) {
                sfColor* pixel = start_pixel + p_i;
                pixel->r = n8[p_i] *  7 % 255;
                pixel->g = n8[p_i] * 11 % 255;
                pixel->b = n8[p_i] * 17 % 255;
                pixel->a = 255;
            }
        }
    }

    return 0;
}

int float8_add(float a[8], float b[8]) {
    for (int i = 0; i < 8; i++) {
        a[i] = a[i] + b[i];
    }

    return 0;
}

int float8_sub(float a[8], float b[8]) {
    for (int i = 0; i < 8; i++) {
        a[i] = a[i] - b[i];
    }

    return 0;
}

int float8_mul(float a[8], float b[8]) {
    for (int i = 0; i < 8; i++) {
        a[i] = a[i] * b[i];
    }

    return 0;
}

int float8_div(float a[8], float b[8]) {
    for (int i = 0; i < 8; i++) {
        a[i] = a[i] / b[i];
    }

    return 0;
}

int float8_rng(float a[8]) {
    for (int i = 0; i < 8; i++) {
        a[i] = (float)i;
    }

    return 0;
}

int float8_fil(float a[8], float b) {
    for (int i = 0; i < 8; i++) {
        a[i] = b;
    }

    return 0;
}

int float8_cpy(float a[8], float b[8]) {
    for (int i = 0; i < 8; i++) {
        a[i] = b[i];
    }

    return 0;
}

int float8_cmp(float a[8], float b[8], int cmp[8]) {
    for (int i = 0; i < 8; i++) {
        cmp[i] = (a[i] > b[i] ? 1 : 0);
    }

    return 0;
}

int is_cmp_zero(int cmp[8]) {
    int is_cmp_zero = true;
    for (int i = 0; i < 8; i++) {
        if (cmp[i] != 0) {
            is_cmp_zero = false;
        }
    }

    return is_cmp_zero;
}

int int8_add(int a[8], int b[8]) {
    for (int i = 0; i < 8; i++) {
        a[i] = a[i] + b[i];
    }

    return 0;
}