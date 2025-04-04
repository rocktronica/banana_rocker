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

const int frameRate = 1;

const int rockingAmplitudeDegrees = 90;
const int rockingFrequencyMs = 1500;
const float momentumDrop = .025;

// TODO: what's the technical term for a side of the swing?
// TODO: try stepsRemainingInRock
const int stepsPerRock = 8; // (frameRate * (rockingFrequencyMs / 1000.0)) / 2;

enum Direction { IDLE, LEFT, RIGHT };

int radius = 25;
int coverage = 170;
int rotation = 0;
int step = 0;
float momentum = 0;
Direction direction = Direction::IDLE;

bool showStats = true;
bool hasPlayedStopSound = true;

// https://github.com/nicolausYes/easing-functions
double easeInOutSine(double t) {
  return 0.5 * (1 + sin(3.1415926 * (t - 0.5)));
}
double easeInOutQuad(double t) {
  return t < 0.5 ? 2 * t * t : t * (4 - 2 * t) - 1;
}
double easeInOutCubic(double t) {
  return t < 0.5 ? 4 * t * t * t : 1 + (--t) * (2 * (--t)) * (2 * t);
}
double easeInOutQuart(double t) {
  if (t < 0.5) {
    t *= t;
    return 8 * t * t;
  } else {
    t = (--t) * t;
    return 1 - 8 * t * t;
  }
}
double easeInOutQuint(double t) {
  double t2;
  if (t < 0.5) {
    t2 = t * t;
    return 16 * t * t2 * t2;
  } else {
    t2 = (--t) * t;
    return 1 + 16 * t * t2 * t2;
  }
}
double easeInOutExpo(double t) {
  if (t < 0.5) {
    return (pow(2, 16 * t) - 1) / 510;
  } else {
    return 1 - 0.5 * pow(2, -16 * (t - 0.5));
  }
}
double easeInOutCirc(double t) {
  if (t < 0.5) {
    return (1 - sqrt(1 - 2 * t)) * 0.5;
  } else {
    return (1 + sqrt(2 * t - 1)) * 0.5;
  }
}

void setup() {
  arduboy.beginDoFirst();
  arduboy.waitNoButtons();
  arduboy.setFrameRate(frameRate);
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
// * extract consts, direction setting
// * rate limit
// * DRY momentum drop
void handleInputs(float initialMomentum = .5, float momentumIncrement = 1.1) {
  arduboy.pollButtons();

  if (arduboy.pressed(LEFT_BUTTON)) {
    if (momentum == 0.0) {
      momentum = initialMomentum;
      direction = Direction::LEFT;
      // } else {
      //   momentum *= momentumIncrement;
    }
  } else if (arduboy.pressed(RIGHT_BUTTON)) {
    if (momentum == 0.0) {
      momentum = initialMomentum;
      direction = Direction::RIGHT;
      // } else {
      //   momentum *= momentumIncrement;
    }
  }

  if (arduboy.pressed(DOWN_BUTTON)) {
    sound.tones(CROUCH_TONES);
    momentum /= 2;

    if (momentum == 0) {
      direction == Direction::IDLE;
      step = 0;
    }
  }

  if (arduboy.justPressed(A_BUTTON)) {
    showStats = !showStats;
  }

  momentum = min(1.25, max(-.25, momentum));
}

inline int getX() { return (rotation / 360.0) * radius * 2 * M_PI; }

void loop() {
  if (!(arduboy.nextFrame())) {
    return;
  }

  float linearDeviation = 0;
  float easedDeviation = 0;

  // fix it getting stuck on last step twice
  if (direction != Direction::IDLE && step < (stepsPerRock - 1) &&
      momentum > 0) {
    linearDeviation = float(step) / (stepsPerRock - 1);
    // linearDeviation = abs((float(step) / (stepsPerRock - 1)) - 1.0 / 2) * 2;

    // easedDeviation = easeInOutSine(linearDeviation); // naw
    // easedDeviation = easeInOutQuad(linearDeviation); // eh
    // easedDeviation = easeInOutCubic(linearDeviation); // disappears?

    // TODO: get this looking at least linearly correct
    // A big part of the problem is that the start and rest are the same,
    // which looks like a pause when 2+ cycles are back to back
    easedDeviation = linearDeviation;

    // None of these are good
    // easedDeviation = easeInOutQuart(linearDeviation);
    // easedDeviation = easeInOutQuint(linearDeviation);
    // easedDeviation = easeInOutExpo(linearDeviation);
    // easedDeviation = easeInOutCirc(linearDeviation);

    // Ugh, now how'd I break this one?
    rotation = momentum * easedDeviation * rockingAmplitudeDegrees;

    if (direction == Direction::LEFT) {
      rotation = rotation - 90;
      // rotation = -rotation;
    } else if (direction == Direction::RIGHT) {
      rotation = 90 - rotation;
      // rotation = rotation;
    }

    step += 1;
  } else {
    step = 0;
    momentum = max(0, momentum - .1);
    rotation = 0;

    if (momentum > 0) {
      if (direction == Direction::LEFT) {
        direction = Direction::RIGHT;
      } else if (direction == Direction::RIGHT) {
        direction = Direction::LEFT;
      }
    } else {
      direction = Direction::IDLE;
    }
  }

  // if ((linearDeviation == .5 && !sound.playing()) ||
  //     momentum == 0 && !hasPlayedStopSound) {
  //   sound.tones(CROUCH_TONES);
  //   hasPlayedStopSound = true;
  // }

  handleInputs();

  arduboy.clear();

  if (showStats) {
    arduboy.setCursor(0, 0);
    arduboy.print(float(momentum));
    arduboy.setCursor(30, 0);
    arduboy.print(easedDeviation);
    arduboy.setCursor(60, 0);
    arduboy.print(rockingAmplitudeDegrees);
    arduboy.setCursor(90, 0);
    arduboy.print(rotation);
  }

  drawBanana(coverage, radius, rotation, center.x + getX(), center.y);
  arduboy.drawFastHLine(0, floorY, WIDTH);

  arduboy.display();
}
