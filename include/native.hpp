#ifndef NATIVE_HPP_
#define NATIVE_HPP_

#include <SFML/Graphics.h>

#include "screen_state.hpp"

int NativeUpdatePixels(ScreenState screen_state, sfColor* pixels);
int GetN(float cx, float cy);

#endif // NATIVE_HPP_