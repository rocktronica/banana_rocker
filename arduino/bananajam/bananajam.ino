#include <Arduboy2.h>

Arduboy2 arduboy;

void setup() {
  arduboy.boot();
  arduboy.setFrameRate(15);
}

int radius = 10;
int coverage = 270;

struct Xy {
  int x = 128 / 2;
  int y = 64 / 2;
} center;

// TODO: fix weird bumps at right and bottom
void drawSemiCircle(int startingAngle, int coverage, int radius,
                    Xy center = center) {
  for (int angle = startingAngle; angle <= startingAngle + coverage; ++angle) {
    float radian = radians(angle);
    int px = center.x + radius * cos(radian);
    int py = center.y + radius * sin(radian);
    arduboy.drawPixel(px, py);
  }
}

void handleInputs() {
  arduboy.pollButtons();
  if (arduboy.pressed(LEFT_BUTTON)) {
    coverage--;
  } else if (arduboy.pressed(RIGHT_BUTTON)) {
    coverage++;
  } else if (arduboy.pressed(UP_BUTTON)) {
    radius++;
  } else if (arduboy.pressed(DOWN_BUTTON)) {
    radius--;
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

  drawSemiCircle(0, coverage, radius);

  arduboy.display();
}
