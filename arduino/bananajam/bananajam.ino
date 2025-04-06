#include <Arduboy2.h>
#include <ArduboyTones.h>
#include <Tinyfont.h>

#include "noise.h"

Arduboy2 arduboy;
ArduboyTones sound(arduboy.audio.enabled);
Tinyfont tinyfont = Tinyfont(arduboy.sBuffer, WIDTH, HEIGHT);

// TODO:
// * optimize variable types
// * see about inlining math functions
// * deal w/ 1+ momentum
// * sound effects
// * count score -> detented progress bar. animate progress?
// * Display struct
// * Banana struct w/ display, momentum, amplitude, frequency; ints > float math
// * -> banana maker to find values, test difficulty
// * try filling body white by keeping track of top/bottomYs

const int frameRate = 30;
const int rockingAmplitudeDegrees = 90;
const int tippingAmplitudeDegrees = rockingAmplitudeDegrees + 1;
const int rockingFrequencyMs = 1000;
const float momentumDropPerFrame = .05;
const float minMomentum = .01;
const float initialMomentum = .1;
const float momentumIncrement = 1.1;
const int stepsPerRock = (frameRate * (rockingFrequencyMs / 1000.0)) / 2;
enum Side { CENTER, LEFT, RIGHT };

int radius;
int coverage;
int rotation;
int step;
float momentum;
float weight;
Side side;
Side direction;
bool isTipping;
bool showStats = false;
float deviation;
int floorY;

struct Xy {
  int x = WIDTH / 2;
  int y = HEIGHT / 2;
} center;

void reset() {
  radius = 25;
  coverage = 170;
  rotation = -tippingAmplitudeDegrees;
  step = stepsPerRock / 2;
  momentum = 1.02;
  weight = 1;
  side = Side::LEFT;
  direction = Side::LEFT;
  isTipping = false;
  // showStats = true;
  deviation = 0;
  floorY = center.y + radius;
}

void setup() {
  arduboy.beginDoFirst();
  arduboy.waitNoButtons();
  arduboy.setFrameRate(frameRate);

  reset();
}

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

// TODO: a little stem should be easy enough
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
      weight = getWeight(Side::LEFT, direction, step, stepsPerRock);
      momentum *= max(1, momentumIncrement * weight);
    }
  } else if (arduboy.pressed(RIGHT_BUTTON)) {
    if (momentum <= minMomentum) {
      momentum = initialMomentum;
      side = Side::RIGHT;
    } else {
      weight = getWeight(Side::RIGHT, direction, step, stepsPerRock);
      momentum *= max(1, momentumIncrement * weight);
    }
  }

  if (arduboy.pressed(DOWN_BUTTON)) {
    sound.tones(CROUCH_TONES);
    slowDown(.5);
  }

  if (arduboy.justPressed(A_BUTTON)) {
    showStats = !showStats;
  }
}

inline int getX() { return (rotation / 360.0) * radius * 2 * M_PI; }

Xy getPosition() {
  Xy xy;

  xy.x = center.x + getX();
  xy.y = center.y;

  if (isTipping) {
    // TODO: DRY
    if (direction == Side::RIGHT) {
      xy.x += sin(radians(rotation)) * radius - radius;
      xy.y -= cos(radians(90 - rotation)) * radius - radius - 1;
    } else {
      xy.x += sin(radians(rotation)) * radius + radius;
      xy.y += cos(radians(90 - rotation)) * radius + radius + 1;
    }
  }

  return xy;
}

// TODO: with a fresh head, really figure out deviation math here
// there always seems to be a weird stutter in there
float getWeight(Side targetDirection, Side currentDirection, int i, int count) {
  if (currentDirection == RIGHT) {
    if (targetDirection == Side::RIGHT) {
      return 1;
    } else if (targetDirection == Side::LEFT) {
      return 0;
    }
  } else if (currentDirection == LEFT) {
    if (targetDirection == Side::RIGHT) {
      return 0;
    } else if (targetDirection == Side::LEFT) {
      return 1;
    }
  }

  return 1;
}

const float getLinearDeviation(Side fromSide, Side toSide, int i, int count) {
  if (fromSide == Side::LEFT) {
    if (toSide == Side::LEFT) {
      return (1 - getLinearDeviation(Side::CENTER, toSide, i, count)) / 2;
    } else if (toSide == Side::RIGHT) {
      return .5 + getLinearDeviation(Side::CENTER, toSide, i, count) / 2;
    }

    return .5;
  } else if (fromSide == Side::RIGHT) {
    return 1 - getLinearDeviation(Side::LEFT, toSide, i, count);
  }

  return 1 - abs((float(i) / count) - .5) * 2;
}

const float getEasedDeviation(Side fromSide, Side toSide, float i, int count) {
  // easeInSine from https://github.com/nicolausYes/easing-functions
  return sin(1.5707963 * getLinearDeviation(fromSide, toSide, i, count));
}

// TODO: implement, require some new minimum momentum?
void scorePoint() {
  // sound.tones(CROUCH_TONES);
}

void slowDown(float drop) {
  momentum = momentum * (1.0 - drop);

  if (momentum <= minMomentum) {
    momentum = 0;
    step = 0;
  }
}

void drawStats() {
  int line = 0;
  int x = 10 * 5;

  tinyfont.setCursor(0, 5 * line);
  tinyfont.print(F("STEP:"));
  tinyfont.setCursor(x, 5 * line);
  tinyfont.print(step);
  line += 1;

  tinyfont.setCursor(0, 5 * line);
  tinyfont.print(F("SIDE:"));
  tinyfont.setCursor(x, 5 * line);
  tinyfont.print(side);
  line += 1;

  tinyfont.setCursor(0, 5 * line);
  tinyfont.print(F("DIRECTION:"));
  tinyfont.setCursor(x, 5 * line);
  tinyfont.print(direction);
  line += 1;

  tinyfont.setCursor(0, 5 * line);
  tinyfont.print(F("ROTATION:"));
  tinyfont.setCursor(x, 5 * line);
  tinyfont.print(rotation);
  line += 1;

  tinyfont.setCursor(0, 5 * line);
  tinyfont.print(F("DEVIATION:"));
  tinyfont.setCursor(x, 5 * line);
  tinyfont.print(deviation);
  line += 1;

  tinyfont.setCursor(0, 5 * line);
  tinyfont.print(F("MOMENTUM:"));
  tinyfont.setCursor(x, 5 * line);
  tinyfont.print(float(momentum));
  line += 1;

  tinyfont.setCursor(0, 5 * line);
  tinyfont.print(F("frmLEFT:"));
  tinyfont.setCursor(x, 5 * line);
  tinyfont.print(getLinearDeviation(Side::LEFT, side, step, stepsPerRock));
  line += 1;

  tinyfont.setCursor(0, 5 * line);
  tinyfont.print(F("frmRIGHT:"));
  tinyfont.setCursor(x, 5 * line);
  tinyfont.print(getLinearDeviation(Side::RIGHT, side, step, stepsPerRock));
  line += 1;

  tinyfont.setCursor(0, 5 * line);
  tinyfont.print(F("WEIGHT:"));
  tinyfont.setCursor(x, 5 * line);
  tinyfont.print(weight);
  line += 1;
}

void updateRotation() {
  deviation = getEasedDeviation(Side::CENTER, side, step, stepsPerRock);

  if (isTipping) {
    deviation = 1 + (1 - deviation); // 0 to 1 -> 1 to 2

    // TODO: deal w/ initial tipping rotation so it doesn't jerk back
    rotation = deviation * 90 * (side == Side::LEFT ? -1 : 1);
  } else {
    rotation = momentum * deviation * rockingAmplitudeDegrees *
               (side == Side::LEFT ? -1 : 1);
  }
}

void update() {
  updateRotation();

  if (isTipping || abs(rotation) >= tippingAmplitudeDegrees) {
    isTipping = true;

    // side = Side::RIGHT;
    direction = side;

    if (step < stepsPerRock) {
      step += 1;
    }

    return;
  }

  if (momentum > 0) {
    step += 1;
    slowDown(momentumDropPerFrame);

    if (step == stepsPerRock / 2) {
      if (side == Side::LEFT) {
        direction = Side::RIGHT;
      } else if (side == Side::RIGHT) {
        direction = Side::LEFT;
      }
    }

    if (step == stepsPerRock) {
      step = 0;

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
    direction = Side::CENTER;
    rotation = 0;
  }
}

void loop() {
  if (!(arduboy.nextFrame())) {
    return;
  }

  update();
  handleInputs();

  if (arduboy.justPressed(LEFT_BUTTON)) {
    step -= 1;
  } else if (arduboy.justPressed(RIGHT_BUTTON)) {
    step += 1;
  } else if (arduboy.justPressed(B_BUTTON)) {
    reset();
  }

  arduboy.clear();

  if (showStats) {
    drawStats();
  }

  Xy position = getPosition();
  drawBanana(coverage, radius, rotation, position.x, position.y);
  arduboy.drawFastHLine(0, floorY, WIDTH);

  arduboy.display();
}
