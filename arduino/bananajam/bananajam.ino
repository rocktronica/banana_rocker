#include <Arduboy2.h>
#include <ArduboyTones.h>
#include <Tinyfont.h>

#include "noise.h"

Arduboy2 arduboy;
ArduboyTones sound(arduboy.audio.enabled);

// TODO:
// * optimize variable types
// * deal w/ 1+ momentum
// * sound effects
// * count score
// * fix buggy initial momentum from 0

const int frameRate = 30;

const int rockingAmplitudeDegrees = 90;
const int rockingFrequencyMs = 1000;
const float momentumDrop = .33;
const float minMomentum = .05;
float initialMomentum = .25;
float momentumIncrement = 1.1;

const int stepsPerRock = (frameRate * (rockingFrequencyMs / 1000.0)) / 2;

enum Side { CENTER, LEFT, RIGHT };

int radius = 25;
int coverage = 170;
int rotation = 0;
int step = 0;
float momentum = 0;
Side side = Side::CENTER;

bool showStats = true;

void setup() {
  arduboy.beginDoFirst();
  arduboy.waitNoButtons();
  arduboy.setFrameRate(frameRate);

  for (int i = 0; i < 10; ++i) {
    Serial.print("i:");
    Serial.print(i);
    Serial.print("\tlinear:");
    Serial.print(getLinearDeviation(i, 10));
    Serial.print("\teased:");
    Serial.print(getEasedDeviation(i, 10));
    Serial.println();
  }
}

struct Xy {
  int x = WIDTH / 2;
  int y = HEIGHT / 2;
} center;

const int floorY = center.y + radius;

// TODO: fix weird bumps at right and bottom
void drawSemiCircle(int startingAngle, int coverage, int radius, int x, int y) {
  for (int angle = (startingAngle + 90);
       angle <= (startingAngle + 90) + coverage; ++angle) {
    float radian = radians(angle);
    int px = x + radius * cos(radian);
    int py = y + radius * sin(radian);
    arduboy.drawPixel(px, py);
  }
}

void drawBanana(int bottomCoverage, int bottomRadius, int rotation, int x,
                int y) {
  // NOTE: unapologetically magic values here, who cares
  int topRadius = bottomRadius * 2;
  int topCoverage = bottomCoverage * .33;
  int depth = bottomRadius * 1.75;
  int midRadius = (topRadius + bottomRadius) / 2;
  int midCoverage = topCoverage;
  int midDepth = depth * .6;

  float angle = radians(rotation - 90);

  drawSemiCircle(rotation - bottomCoverage / 2, bottomCoverage, bottomRadius, x,
                 y);
  drawSemiCircle(rotation - topCoverage / 2, topCoverage, topRadius,
                 x + depth * cos(angle), y + depth * sin(angle));

  drawSemiCircle(rotation - midCoverage / 2, midCoverage, midRadius,
                 x + midDepth * cos(angle), y + midDepth * sin(angle));
}

// TODO:
// * redo slowDown
// * tidy, extract consts + side stuff
// * rate limit
void handleInputs() {
  arduboy.pollButtons();

  if (arduboy.pressed(LEFT_BUTTON)) {
    if (momentum <= minMomentum) {
      momentum = initialMomentum;
      side = Side::LEFT;
    } else {
      momentum *= momentumIncrement;
    }
  } else if (arduboy.pressed(RIGHT_BUTTON)) {
    if (momentum <= minMomentum) {
      momentum = initialMomentum;
      side = Side::RIGHT;
    } else {
      momentum *= momentumIncrement;
    }
  }

  if (arduboy.pressed(DOWN_BUTTON)) {
    sound.tones(CROUCH_TONES);
    slowDown();
  }

  if (arduboy.justPressed(A_BUTTON)) {
    showStats = !showStats;
  }

  momentum = min(1.25, max(-.25, momentum));
}

inline int getX() { return (rotation / 360.0) * radius * 2 * M_PI; }

const float getLinearDeviation(int i, int count) {
  return 1 - abs((float(i) / count) - .5) * 2;
}

const float getEasedDeviation(float i, int count) {
  // easeInSine from https://github.com/nicolausYes/easing-functions
  return sin(1.5707963 * (1 - abs((float(i) / count) - .5) * 2));
}

// TODO: implement
void scorePoint() { sound.tones(CROUCH_TONES); }

void slowDown() {
  momentum = momentum * (1.0 - momentumDrop);

  if (momentum <= minMomentum) {
    momentum = 0;
    step = 0;
  }
}

void loop() {
  if (!(arduboy.nextFrame())) {
    return;
  }

  float linearDeviation = 0;
  float easedDeviation = 0;

  linearDeviation = getLinearDeviation(step, stepsPerRock);
  easedDeviation = getEasedDeviation(step, stepsPerRock);

  rotation = momentum * easedDeviation * rockingAmplitudeDegrees;
  if (side == Side::LEFT) {
    rotation = -rotation;
  }

  if (momentum > 0) {
    step += 1;

    // TODO: try slowDown more frequently

    if (step == stepsPerRock) {
      step = 0;
      slowDown();

      if (momentum > minMomentum) {
        scorePoint();

        if (side == Side::LEFT) {
          side = Side::RIGHT;
        } else if (side == Side::RIGHT) {
          side = Side::LEFT;
        }
      }
    }
  } else {
    side = Side::CENTER;
    rotation = 0;
  }

  handleInputs();

  arduboy.clear();

  if (showStats) {
    arduboy.setCursor(0, 0);
    arduboy.print(step);
    arduboy.setCursor(20, 0);
    arduboy.print(side);
    arduboy.setCursor(30, 0);
    arduboy.print(float(momentum));
    arduboy.setCursor(60, 0);
    arduboy.print(easedDeviation);
    arduboy.setCursor(90, 0);
    arduboy.print(rockingAmplitudeDegrees);
  }

  drawBanana(coverage, radius, rotation, center.x + getX(), center.y);
  arduboy.drawFastHLine(0, floorY, WIDTH);

  arduboy.display();
}
