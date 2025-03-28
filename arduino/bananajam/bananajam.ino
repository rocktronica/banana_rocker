#include <Arduboy2.h>

Arduboy2 arduboy;

void setup() {
  arduboy.boot();
  arduboy.setFrameRate(15);
}

int radius = 20;
int coverage = 180;

struct Xy {
  int x = 128 / 2;
  int y = 64 / 2;
} center;

// TODO: fix weird bumps at right and bottom
void drawSemiCircle(int startingAngle, int coverage, int radius, int x, int y) {
  for (int angle = startingAngle; angle <= startingAngle + coverage; ++angle) {
    float radian = radians(angle);
    int px = x + radius * cos(radian);
    int py = y + radius * sin(radian);
    arduboy.drawPixel(px, py);
  }
}

void drawBanana(int coverage, int bottomRadius, int x, int y) {
  // TODO: parameterize/derive
  int topRadius = bottomRadius * 2;
  int topCoverage = coverage * .33;
  int depth = bottomRadius * 1.75;

  drawSemiCircle(90 - coverage / 2, coverage, bottomRadius, x, y);
  drawSemiCircle(90 - topCoverage / 2, topCoverage, topRadius, x, y - depth);
}

void handleInputs(int coverageIncrement = 10, int radiusIncrement = 1) {
  arduboy.pollButtons();
  if (arduboy.pressed(LEFT_BUTTON)) {
    coverage -= coverageIncrement;
  } else if (arduboy.pressed(RIGHT_BUTTON)) {
    coverage += coverageIncrement;
  } else if (arduboy.pressed(UP_BUTTON)) {
    radius += radiusIncrement;
  } else if (arduboy.pressed(DOWN_BUTTON)) {
    radius -= radiusIncrement;
  }
}

void loop() {
  if (!(arduboy.nextFrame())) {
    return;
  }

  handleInputs();

  arduboy.clear();

  arduboy.setCursor(0, 0);
  arduboy.print(radius);
  arduboy.setCursor(40, 0);
  arduboy.print(coverage);

  drawBanana(coverage, radius, center.x, center.y);

  arduboy.display();
}
