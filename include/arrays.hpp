#ifndef ARRAYS_HPP_
#define ARRAYS_HPP_

#include <SFML/Graphics.h>

#include "screen_state.hpp"

int ArraysUpdatePixels(ScreenState screen_state, sfColor* pixels);

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

#endif // ARRAYS_HPP_