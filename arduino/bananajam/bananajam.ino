#include <Arduboy2.h>

Arduboy2 arduboy;

// TODO: optimize variable types

const int frameRate = 24;

const int rockingAmplitudeDegrees = 90;
const int rockingFrequencyMs = 1500;

const int stepsPerRock = frameRate * (rockingFrequencyMs / 1000.0);

int radius = 25;
int coverage = 170;
int rotation = 0;
int step = 0;
float momentum = 1;

void setup() {
  arduboy.boot();
  arduboy.setFrameRate(frameRate);
}

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

void drawBanana(int coverage, int bottomRadius, int rotation, int x, int y) {
  // TODO: parameterize/derive
  int topRadius = bottomRadius * 2;
  int topCoverage = coverage * .33;
  int depth = bottomRadius * 1.75;

  drawSemiCircle(rotation + 90 - coverage / 2, coverage, bottomRadius, x, y);

  // TODO: extract and duplicate for texture
  float angle = radians(rotation - 90);
  drawSemiCircle(rotation + 90 - topCoverage / 2, topCoverage, topRadius,
                 x + depth * cos(angle), y + depth * sin(angle));
}

void handleInputs() {
  arduboy.pollButtons();

  if (arduboy.pressed(LEFT_BUTTON)) {
    rotation -= 15;
  } else if (arduboy.pressed(RIGHT_BUTTON)) {
    rotation += 15;
  }

  // TODO: contain momentum from 0 to 1
  if (arduboy.pressed(DOWN_BUTTON)) {
    momentum -= .1;
  } else if (arduboy.pressed(UP_BUTTON)) {
    momentum += .1;
  }
}

inline int getX() { return (rotation / 360.0) * radius * 2 * M_PI; }

void loop() {
  if (!(arduboy.nextFrame())) {
    return;
  }

  step = (step + 1) % stepsPerRock;

  float linearDeviation = abs((float(step) / stepsPerRock) - 1.0 / 2) * 2;
  // TODO: easedDeviation

  rotation = momentum * (linearDeviation * rockingAmplitudeDegrees * 2 -
                         rockingAmplitudeDegrees);

  handleInputs();

  arduboy.clear();

  arduboy.setCursor(0, 0);
  arduboy.print(linearDeviation);
  arduboy.setCursor(40, 0);
  arduboy.print(rotation);
  arduboy.setCursor(80, 0);
  arduboy.print(momentum);

  drawBanana(coverage, radius, rotation, center.x + getX(), center.y);

  arduboy.display();
}
