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

inline Color white{255, 255, 255, 255};

inline Color orange{241, 89, 41};

inline Color mid_gray{101, 101, 101};

inline Color black{0, 0, 0};

inline Color green{0, 186, 81};

inline Color red{255, 0, 0};

struct Point {
  uint16_t x;
  uint16_t y;
};

#endif // UI_H
