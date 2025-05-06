//
// Created by Nova Mondal on 4/30/25.
//

#ifndef UI_H
#define UI_H

struct Color {
  uint8_t red;
  uint8_t green;
  uint8_t blue;
  uint8_t alpha = 255;
};

inline Color WHITE{255, 255, 255, 255};

struct Point {
  uint16_t x;
  uint16_t y;
};

#endif // UI_H
