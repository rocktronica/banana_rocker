#include <Arduboy2.h>
#include <ArduboyTones.h>
#include <Tinyfont.h>

#include "noise.h"

Arduboy2 arduboy;
ArduboyTones sound(arduboy.audio.enabled);
Tinyfont tinyfont = Tinyfont(arduboy.sBuffer, WIDTH, HEIGHT);

// TODO:
// * title
// * transition into new game after tip
// * optimize variable types
// * see about inlining math functions
// * refine sound effects
// * levels?
// * Display struct
// * Banana struct w/ display, momentum, amplitude, frequency; ints > float math
// * -> banana maker to find values, test difficulty
// * try filling body white by keeping track of top/bottomYs

const int frameRate = 30;
const int rockingAmplitudeDegrees = 90;
const int tippingAmplitudeDegrees = rockingAmplitudeDegrees + 1;
const int rockingFrequencyMs = 1000;
const float momentumDropPerFrame = .05;
const float minMomentumToStart = .01;
const float minMomentumToScore = .1;
const float initialMomentum = .1;
const float momentumIncrement = 1.1;
const int stepsPerRock = (frameRate * (rockingFrequencyMs / 1000.0)) / 2;
const int scoreY = HEIGHT - 4;
const int floorY = scoreY - 2;
const int scoreIncrementMax = 10;

enum Side { CENTER, LEFT, RIGHT, UP, DOWN };

int radius;
int coverage;
int rotation;
int controlledRotation;
int step;
float momentum;
float weight;
Side side;
Side direction;
Side hold;
bool isTipping;
bool isActive;
bool showStats = false;
float deviation;
int score;
int scoreDisplayed = 0;
int scoreBest = 0;
int gamesPlayed = 0;

struct Xy {
  int x;
  int y;
};

void reset() {
  radius = 25;
  coverage = 170;
  rotation = 0;
  controlledRotation = 0;
  step = 0;
  momentum = 0;
  weight = 1;
  side = Side::CENTER;
  direction = Side::CENTER;
  isTipping = false;
  isActive = true;
  deviation = 0;
  score = 0;
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

void startMomentum(Side _side) {
  score = 0;
  momentum = initialMomentum;
  side = _side;
  gamesPlayed += 1;
}

void handleInputs() {
  arduboy.pollButtons();

  if (arduboy.pressed(LEFT_BUTTON)) {
    hold = Side::LEFT;
  } else if (arduboy.pressed(RIGHT_BUTTON)) {
    hold = Side::RIGHT;
  } else if (arduboy.pressed(UP_BUTTON)) {
    hold = Side::UP;
  } else if (arduboy.pressed(DOWN_BUTTON)) {
    hold = Side::DOWN;
  } else {
    hold = Side::CENTER;
  }

  if (arduboy.justPressed(A_BUTTON)) {
    showStats = !showStats;
  }

  if (!isActive) {
    if (arduboy.justPressed(A_BUTTON | B_BUTTON | RIGHT_BUTTON | LEFT_BUTTON |
                            DOWN_BUTTON | UP_BUTTON)) {
      reset();
      scoreDisplayed = 0;
    }

    return;
  }

  if (arduboy.anyPressed(RIGHT_BUTTON | LEFT_BUTTON)) {
    if (momentum <= minMomentumToStart) {
      startMomentum(hold);
    } else {
      // TODO: consider updateMomentum()
      weight = getWeight(hold, direction, step, stepsPerRock);
      momentum *= max(1, momentumIncrement * weight);
    }
  }

  if (momentum > minMomentumToStart) {
    if (arduboy.anyPressed(DOWN_BUTTON | UP_BUTTON)) {
      if (arduboy.pressed(DOWN_BUTTON)) {
        sound.tones(SLOW);
      }

      slowDown(momentumDropPerFrame * 4);

      if (!isTipping) {
        score = max(0, score - 1);
      }
    }
  }
}

Xy getPosition() {
  Xy xy;

  xy.x = WIDTH / 2 + (rotation / 360.0) * radius * 2 * M_PI;
  xy.y = floorY - radius;

  if (isTipping) {
    xy.x += sin(radians(rotation)) * radius +
            (direction == Side::RIGHT ? -radius : radius);

    xy.y += cos(radians(90 - rotation)) *
                (radius * (direction == Side::RIGHT ? -1 : 1)) +
            radius + 1;
  }

  if (hold == Side::RIGHT) {
    xy.x += 1;
  } else if (hold == Side::LEFT) {
    xy.x -= 1;
  }

  if (hold == Side::DOWN) {
    xy.y += 1;
  } else if (hold == Side::UP) {
    xy.y -= 1;
  }

  return xy;
}

// This could be smarter and use more math but I found it doesn't make
// much of a difference in play. Simple seems fine for now.
float getWeight(Side targetDirection, Side currentDirection, int i, int count) {
  if (targetDirection != currentDirection) {
    return 0.0;
  }

  return 1.0;
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

void scorePoint() {
  if (momentum >= minMomentumToScore) {
    sound.tones(SCORE);
    score += momentum * scoreIncrementMax;
    scoreBest = max(score, scoreBest);
  }
}

void slowDown(float drop) {
  momentum = momentum * (1.0 - drop);

  if (momentum <= minMomentumToStart) {
    momentum = 0;
    step = 0;
  }
}

void drawStats() {
  int line = 0;
  int x = 10 * 5;

  tinyfont.setCursor(0, 5 * line);
  tinyfont.print(F("GAMES:"));
  tinyfont.setCursor(x, 5 * line);
  tinyfont.print(gamesPlayed);
  line += 1;

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
  tinyfont.print(F("TIPPING:"));
  tinyfont.setCursor(x, 5 * line);
  tinyfont.print(isTipping);
  line += 1;

  tinyfont.setCursor(0, 5 * line);
  tinyfont.print(F("ROTATION:"));
  tinyfont.setCursor(x, 5 * line);
  tinyfont.print(rotation);
  line += 1;

  tinyfont.setCursor(0, 5 * line);
  tinyfont.print(F("ctrlROT:"));
  tinyfont.setCursor(x, 5 * line);
  tinyfont.print(controlledRotation);
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
  tinyfont.print(F("HOLD:"));
  tinyfont.setCursor(x, 5 * line);
  tinyfont.print(hold);
  line += 1;

  tinyfont.setCursor(0, 5 * line);
  tinyfont.print(F("WEIGHT:"));
  tinyfont.setCursor(x, 5 * line);
  tinyfont.print(weight);
  line += 1;
}

void drawScore() {
  int scoreBestDigits = 1;
  if (scoreBest >= 100) {
    scoreBestDigits = 3;
  } else if (scoreBest >= 10) {
    scoreBestDigits = 2;
  }

  tinyfont.setCursor(5 * 0, scoreY);
  tinyfont.print(F("SCORE"));
  tinyfont.setCursor(5 * 5 + 1, scoreY);
  tinyfont.print(scoreDisplayed);

  if (gamesPlayed > 1 || score < scoreBest) {
    tinyfont.setCursor(WIDTH - (5 * (4 + scoreBestDigits) + 1), scoreY);
    tinyfont.print(F("BEST"));
    tinyfont.setCursor(WIDTH - (5 * scoreBestDigits) + 1, scoreY);

    // TODO: fix brief glimpse of score when new best is made
    tinyfont.print(score == scoreBest ? scoreDisplayed : scoreBest);
  }
}

void updateRotation() {
  deviation = getEasedDeviation(Side::CENTER, side, step, stepsPerRock);

  if (isTipping) {
    deviation = 1 - deviation; // 0 to 1 -> 1 to 2
    rotation = controlledRotation + deviation *
                                        (180 - abs(controlledRotation)) *
                                        (side == Side::LEFT ? -1 : 1);
  } else {
    rotation = momentum * deviation * rockingAmplitudeDegrees *
               (side == Side::LEFT ? -1 : 1);
    controlledRotation = rotation;
  }
}

void update() {
  updateRotation();

  if (score > scoreDisplayed) {
    scoreDisplayed += 1;
  } else if (score < scoreDisplayed) {
    scoreDisplayed -= 1;
  }

  if (isTipping || abs(rotation) >= tippingAmplitudeDegrees) {
    if (!isTipping) {
      // No matter where we are in the current rocking swing, if
      // we're about to tip, reset current step to give the falling
      // animation a regular half cycle to complete.
      step = ceil(stepsPerRock / 2.0);
    }

    isTipping = true;
    direction = side;

    if (step < stepsPerRock) {
      step += 1;
    } else {
      if (isActive) {
        sound.tones(BUMP);
      }

      if (isActive) {
        gamesPlayed += 1;
      }

      isActive = false;
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

      if (momentum > minMomentumToStart) {
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

  arduboy.clear();

  if (showStats) {
    drawStats();
  }

  Xy position = getPosition();
  drawBanana(coverage, radius, rotation, position.x, position.y);

  arduboy.drawFastHLine(0, floorY, WIDTH);
  drawScore();

  arduboy.display();
}
