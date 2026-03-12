#ifndef INPUT_H
#define INPUT_H
#include "raylib.h"

bool isOneKeyPressed(const KeyboardKey *keys, int keys_length);
bool isOneKeyDown(const KeyboardKey *keys, int keys_length);
bool isOneKeyReleased(const KeyboardKey *keys, int keys_length);

#endif // INPUT_H
