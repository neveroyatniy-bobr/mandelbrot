#ifndef CMD_ARGS_HANDLER_HPP_
#define CMP_ARGS_HANDLER_HPP_

#include <SFML/Graphics.h>

#include "screen_state.hpp"

const int INVALID_MODE = -1;

void HandleCmdArgs(const int argc, char* const argv[], int (**UpdatePixels)(ScreenState, sfColor*));

#endif // CMD_ARGS_HANDLER_