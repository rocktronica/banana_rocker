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

const int frameRate = 24;

const int rockingAmplitudeDegrees = 90;
const int rockingFrequencyMs = 1500;
const float momentumDropPerStep = .0125;

const int stepsPerRock = frameRate * (rockingFrequencyMs / 1000.0);
const int leftwardStepIndex = stepsPerRock / 2;
const int rightwardStepIndex = 0;

int radius = 25;
int coverage = 170;
int rotation = 0;
int step = 0;
float momentum = 0;
int direction = 0; // -1 = left, 1 = right

bool showStats = false;
bool hasPlayedStopSound = true;

// https://github.com/nicolausYes/easing-functions
double easeInOutQuad(double t) {
  return t < 0.5 ? 2 * t * t : t * (4 - 2 * t) - 1;
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
void handleInputs(float initialMomentum = .25, float momentumIncrement = 1.1) {
  arduboy.pollButtons();

  if (arduboy.pressed(LEFT_BUTTON)) {
    // if (!sound.playing()) {
    //   sound.tones(MOVE_TONES);
    // }

    if (momentum == 0.0) {
      momentum = initialMomentum;
      direction = -1;
      step = leftwardStepIndex - stepsPerRock / 4;
    } else if (direction == 1) {
      momentum *= (momentumIncrement - 1);
    } else {
      momentum *= momentumIncrement;
    }
  } else if (arduboy.pressed(RIGHT_BUTTON)) {
    // if (!sound.playing()) {
    //   sound.tones(MOVE_TONES);
    // }

    // TODO: fix initial rightward movement not matching leftward
    if (momentum == 0.0) {
      momentum = initialMomentum;
      direction = 1;
      step = rightwardStepIndex - stepsPerRock / 4;
    } else if (direction == -1) {
      momentum *= (momentumIncrement - 1);
    } else {
      momentum *= momentumIncrement;
    }
  }

  if (arduboy.pressed(DOWN_BUTTON)) {
    sound.tones(CROUCH_TONES);
    momentum /= 2;
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

  if (momentum > 0 || rotation != 0) {
    step = (step + 1) % stepsPerRock;
    momentum = max(0, momentum - momentumDropPerStep);

    linearDeviation = abs((float(step) / stepsPerRock) - 1.0 / 2) * 2;
    easedDeviation = easeInOutQuad(linearDeviation);

    direction = step < leftwardStepIndex ? -1 : 1;

    rotation = momentum * (easedDeviation * rockingAmplitudeDegrees * 2 -
                           rockingAmplitudeDegrees);
  } else {
    direction = 0;
  }

  if ((linearDeviation == .5 && !sound.playing()) ||
      momentum == 0 && !hasPlayedStopSound) {
    sound.tones(CROUCH_TONES);
    hasPlayedStopSound = true;
  }

  handleInputs();

  arduboy.clear();

  if (showStats) {
    arduboy.setCursor(0, 0);
    arduboy.print(linearDeviation);
    arduboy.setCursor(30, 0);
    arduboy.print(sound.playing());
    arduboy.setCursor(60, 0);
    arduboy.print(direction);
    arduboy.setCursor(90, 0);
    arduboy.print(momentum);
  }

  drawBanana(coverage, radius, rotation, center.x + getX(), center.y);
  arduboy.drawFastHLine(0, floorY, WIDTH);

  arduboy.display();
}
