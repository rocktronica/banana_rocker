#pragma once

#include <Arduboy2.h>

#include "banana.h"

// NOTE: startingAngle is clockwise from bottom
void drawSemiCircle(int startingAngle, int arc, int radius, uint8_t color,
                    int x, int y) {
  for (int angle = (startingAngle + 90); angle <= (startingAngle + 90) + arc;
       ++angle) {
    float radian = radians(angle);
    int px = x + radius * cos(radian);
    int py = y + radius * sin(radian);

    // HACK!: for math reasons I don't know or care about, the bottom and right
    // pixels both stick out one. This very dumbly nudges them back in.
    if (angle == 0 || angle == 360) {
      px -= 1;
    } else if (angle == 90) {
      py -= 1;
    }

    Arduboy2Base::drawPixel(px, py, color);
  }
}

void drawBanana(Banana banana, int rotation, int x, int y) {
  float radian = radians(rotation - 90);

  Arduboy2Base::fillCircle(x, y, banana.outerRadius);
  Arduboy2Base::fillCircle(x + banana.depth * cos(radian),
                           y + banana.depth * sin(radian), banana.innerRadius,
                           BLACK);

  //  Inner accent
  drawSemiCircle(rotation - banana.accentArc / 2, banana.accentArc,
                 banana.accentRadius, BLACK,
                 x + banana.accentDepth * cos(radian),
                 y + banana.accentDepth * sin(radian));

  // Stem
  drawSemiCircle(rotation + banana.outerArc / 2 - banana.stemOverlap,
                 banana.stemLength, banana.outerRadius, WHITE, x, y);
  for (int i = 1; i < banana.stemDepth - 1; i++) {
    drawSemiCircle(rotation + banana.outerArc / 2 - banana.stemOverlap,
                   banana.stemLength, banana.outerRadius - i, BLACK, x, y);
    drawSemiCircle(rotation + banana.outerArc / 2 - banana.stemOverlap +
                       banana.stemLength,
                   1, banana.outerRadius - i, WHITE, x, y);
  }
  drawSemiCircle(rotation + banana.outerArc / 2 - banana.stemOverlap,
                 banana.stemLength, banana.outerRadius - (banana.stemDepth - 1),
                 WHITE, x, y);
}