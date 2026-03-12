#include "input.h"

bool isOneKeyPressed(const KeyboardKey *keys, int keys_length) {
  for (int i = 0; i < keys_length; ++i) {
    if (IsKeyPressed(keys[i]))
      return true;
  }
  return false;
}

bool isOneKeyDown(const KeyboardKey *keys, int keys_length) {
  for (int i = 0; i < keys_length; ++i) {
    if (IsKeyDown(keys[i]))
      return true;
  }
  return false;
}

bool isOneKeyReleased(const KeyboardKey *keys, int keys_length) {
  for (int i = 0; i < keys_length; ++i) {
    if (IsKeyReleased(keys[i]))
      return true;
  }
  return false;
}
