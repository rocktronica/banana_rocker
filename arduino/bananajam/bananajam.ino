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
// * banana maker to find values, test difficulty
// * try overlapping circles for filled banana

enum Side { CENTER, LEFT, RIGHT, UP, DOWN };

struct Xy {
  int x;
  int y;
};

struct Animation {
  const int frameRate = 30;
  const int rockingFrequencyMs = 1000;
  const float momentumDropPerFrame = .05;

  int stepsPerRock;
} animation;

struct Game {
  const float minMomentumToStart = .01;
  const float minMomentumToScore = .1;
  const int scoreIncrementMax = 10;

  int score = 0;
  int scoreDisplayed = 0;
  int scoreBest = 0;
  int gamesPlayed = 0;

  // TODO: {idle, active, tipping, flipped}
  bool isActive = false;
  bool isTipping = false;
} game;

// Some of these might make more sense in other structs.
// Maybe Input or Game? See what feels weird.
struct Display {
  const int floorY = HEIGHT - 6;
  const int scoreY = HEIGHT - 4;

  bool showStats = false;

  int controlledRotation = 0;
  int rotation = 0;

  float momentum = 0;
  float deviation = 0;
  Side direction = Side::CENTER;
  Side side = Side::CENTER;

  int step = 0;
  float weight = 0;
} display;

struct Input {
  Side hold;
} input;

// NOTE: unapologetically magic values here, who cares
struct Banana {
  const int rockingAmplitudeDegrees = 90;
  const int tippingAmplitudeDegrees = 91;

  const float initialMomentum = .1;
  const float momentumIncrement = 1.1;

  const int outerRadius = 25;
  const int outerArc = 170;

  const int innerRadius = 50;
  const int innerArc = 56;
  const int depth = 43;
  const int accentRadius = 37;
  const int accentArc = 43;
  const int accentDepth = 25;
} banana;

void reset() {
  display.controlledRotation = 0;
  display.rotation = 0;

  display.momentum = 0;
  display.deviation = 0;
  display.direction = Side::CENTER;
  display.side = Side::CENTER;

  display.step = 0;
  display.weight = 1;

  game.score = 0;
  game.isTipping = false;
  game.isActive = true;

  animation.stepsPerRock =
      (animation.frameRate * (animation.rockingFrequencyMs / 1000.0)) / 2;
}

void setup() {
  arduboy.beginDoFirst();
  arduboy.waitNoButtons();
  arduboy.setFrameRate(animation.frameRate);

  reset();
}

// TODO: fix weird bumps at right and bottom
void drawSemiCircle(int startingAngle, int outerArc, int outerRadius, int x,
                    int y) {
  for (int angle = (startingAngle + 90);
       angle <= (startingAngle + 90) + outerArc; ++angle) {
    float radian = radians(angle);
    int px = x + outerRadius * cos(radian);
    int py = y + outerRadius * sin(radian);
    arduboy.drawPixel(px, py);
  }
}

// TODO: Banana as argument
// TODO: a little stem should be easy enough
void drawBanana(int bottomArc, int bottomRadius, int rotation, int x, int y) {
  float radian = radians(display.rotation - 90);

  drawSemiCircle(display.rotation - bottomArc / 2, bottomArc, bottomRadius, x,
                 y);
  drawSemiCircle(display.rotation - banana.innerArc / 2, banana.innerArc,
                 banana.innerRadius, x + banana.depth * cos(radian),
                 y + banana.depth * sin(radian));

  drawSemiCircle(display.rotation - banana.accentArc / 2, banana.accentArc,
                 banana.accentRadius, x + banana.accentDepth * cos(radian),
                 y + banana.accentDepth * sin(radian));
}

void startMomentum(Side side) {
  game.gamesPlayed += 1;
  game.score = 0;

  display.momentum = banana.initialMomentum;
  display.side = side;
}

void handleInputs() {
  arduboy.pollButtons();

  if (arduboy.pressed(LEFT_BUTTON)) {
    input.hold = Side::LEFT;
  } else if (arduboy.pressed(RIGHT_BUTTON)) {
    input.hold = Side::RIGHT;
  } else if (arduboy.pressed(UP_BUTTON)) {
    input.hold = Side::UP;
  } else if (arduboy.pressed(DOWN_BUTTON)) {
    input.hold = Side::DOWN;
  } else {
    input.hold = Side::CENTER;
  }

  if (arduboy.justPressed(A_BUTTON)) {
    display.showStats = !display.showStats;
  }

  if (!game.isActive) {
    if (arduboy.justPressed(A_BUTTON | B_BUTTON | RIGHT_BUTTON | LEFT_BUTTON |
                            DOWN_BUTTON | UP_BUTTON)) {
      reset();
      game.scoreDisplayed = 0;
    }

    return;
  }

  if (arduboy.anyPressed(RIGHT_BUTTON | LEFT_BUTTON)) {
    if (display.momentum <= game.minMomentumToStart) {
      startMomentum(input.hold);
    } else {
      // TODO: consider updateMomentum()
      display.weight = getWeight(input.hold, display.direction, display.step,
                                 animation.stepsPerRock);
      display.momentum *= max(1, banana.momentumIncrement * display.weight);
    }
  }

  if (display.momentum > game.minMomentumToStart) {
    if (arduboy.anyPressed(DOWN_BUTTON | UP_BUTTON)) {
      if (arduboy.pressed(DOWN_BUTTON)) {
        sound.tones(SLOW);
      }

      slowDown(animation.momentumDropPerFrame * 4);

      if (!game.isTipping) {
        game.score = max(0, game.score - 1);
      }
    }
  }
}

Xy getPosition() {
  Xy xy;

  xy.x = WIDTH / 2 + (display.rotation / 360.0) * banana.outerRadius * 2 * M_PI;
  xy.y = display.floorY - banana.outerRadius;

  if (game.isTipping) {
    xy.x += sin(radians(display.rotation)) * banana.outerRadius +
            (display.direction == Side::RIGHT ? -banana.outerRadius
                                              : banana.outerRadius);

    xy.y +=
        cos(radians(90 - display.rotation)) *
            (banana.outerRadius * (display.direction == Side::RIGHT ? -1 : 1)) +
        banana.outerRadius + 1;
  }

  if (input.hold == Side::RIGHT) {
    xy.x += 1;
  } else if (input.hold == Side::LEFT) {
    xy.x -= 1;
  }

  if (input.hold == Side::DOWN) {
    xy.y += 1;
  } else if (input.hold == Side::UP) {
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
  if (display.momentum >= game.minMomentumToScore) {
    sound.tones(SCORE);
    game.score += display.momentum * game.scoreIncrementMax;
    game.scoreBest = max(game.score, game.scoreBest);
  }
}

void slowDown(float drop) {
  display.momentum = display.momentum * (1.0 - drop);

  if (display.momentum <= game.minMomentumToStart) {
    display.momentum = 0;
    display.step = 0;
  }
}

void drawStats() {
  int line = 0;
  int x = 10 * 5;

  tinyfont.setCursor(0, 5 * line);
  tinyfont.print(F("GAMES:"));
  tinyfont.setCursor(x, 5 * line);
  tinyfont.print(game.gamesPlayed);
  line += 1;

  tinyfont.setCursor(0, 5 * line);
  tinyfont.print(F("STEP:"));
  tinyfont.setCursor(x, 5 * line);
  tinyfont.print(display.step);
  line += 1;

  tinyfont.setCursor(0, 5 * line);
  tinyfont.print(F("SIDE:"));
  tinyfont.setCursor(x, 5 * line);
  tinyfont.print(display.side);
  line += 1;

  tinyfont.setCursor(0, 5 * line);
  tinyfont.print(F("DIRECTION:"));
  tinyfont.setCursor(x, 5 * line);
  tinyfont.print(display.direction);
  line += 1;

  tinyfont.setCursor(0, 5 * line);
  tinyfont.print(F("TIPPING:"));
  tinyfont.setCursor(x, 5 * line);
  tinyfont.print(game.isTipping);
  line += 1;

  tinyfont.setCursor(0, 5 * line);
  tinyfont.print(F("ROTATION:"));
  tinyfont.setCursor(x, 5 * line);
  tinyfont.print(display.rotation);
  line += 1;

  tinyfont.setCursor(0, 5 * line);
  tinyfont.print(F("ctrlROT:"));
  tinyfont.setCursor(x, 5 * line);
  tinyfont.print(display.controlledRotation);
  line += 1;

  tinyfont.setCursor(0, 5 * line);
  tinyfont.print(F("DEVIATION:"));
  tinyfont.setCursor(x, 5 * line);
  tinyfont.print(display.deviation);
  line += 1;

  tinyfont.setCursor(0, 5 * line);
  tinyfont.print(F("MOMENTUM:"));
  tinyfont.setCursor(x, 5 * line);
  tinyfont.print(float(display.momentum));
  line += 1;

  tinyfont.setCursor(0, 5 * line);
  tinyfont.print(F("HOLD:"));
  tinyfont.setCursor(x, 5 * line);
  tinyfont.print(input.hold);
  line += 1;

  tinyfont.setCursor(0, 5 * line);
  tinyfont.print(F("WEIGHT:"));
  tinyfont.setCursor(x, 5 * line);
  tinyfont.print(display.weight);
  line += 1;
}

void drawScore() {
  int scoreBestDigits = 1;
  if (game.scoreBest >= 100) {
    scoreBestDigits = 3;
  } else if (game.scoreBest >= 10) {
    scoreBestDigits = 2;
  }

  tinyfont.setCursor(5 * 0, display.scoreY);
  tinyfont.print(F("SCORE"));
  tinyfont.setCursor(5 * 5 + 1, display.scoreY);
  tinyfont.print(game.scoreDisplayed);

  if (game.gamesPlayed > 1 || game.score < game.scoreBest) {
    tinyfont.setCursor(WIDTH - (5 * (4 + scoreBestDigits) + 1), display.scoreY);
    tinyfont.print(F("BEST"));
    tinyfont.setCursor(WIDTH - (5 * scoreBestDigits) + 1, display.scoreY);

    // TODO: fix brief glimpse of score when new best is made
    tinyfont.print(game.score == game.scoreBest ? game.scoreDisplayed
                                                : game.scoreBest);
  }
}

void updateRotation() {
  display.deviation = getEasedDeviation(Side::CENTER, display.side,
                                        display.step, animation.stepsPerRock);

  if (game.isTipping) {
    display.deviation = 1 - display.deviation; // 0 to 1 -> 1 to 2
    display.rotation = display.controlledRotation +
                       display.deviation *
                           (180 - abs(display.controlledRotation)) *
                           (display.side == Side::LEFT ? -1 : 1);
  } else {
    display.rotation = display.momentum * display.deviation *
                       banana.rockingAmplitudeDegrees *
                       (display.side == Side::LEFT ? -1 : 1);
    display.controlledRotation = display.rotation;
  }
}

void update() {
  updateRotation();

  if (game.score > game.scoreDisplayed) {
    game.scoreDisplayed += 1;
  } else if (game.score < game.scoreDisplayed) {
    game.scoreDisplayed -= 1;
  }

  if (game.isTipping ||
      abs(display.rotation) >= banana.tippingAmplitudeDegrees) {
    if (!game.isTipping) {
      // No matter where we are in the current rocking swing, if
      // we're about to tip, reset current display.step to give the falling
      // animation a regular half cycle to complete.
      display.step = ceil(animation.stepsPerRock / 2.0);
    }

    game.isTipping = true;
    display.direction = display.side;

    if (display.step < animation.stepsPerRock) {
      display.step += 1;
    } else {
      if (game.isActive) {
        sound.tones(BUMP);
      }

      if (game.isActive) {
        game.gamesPlayed += 1;
      }

      game.isActive = false;
    }

    return;
  }

  if (display.momentum > 0) {
    display.step += 1;
    slowDown(animation.momentumDropPerFrame);

    if (display.step == animation.stepsPerRock / 2) {
      if (display.side == Side::LEFT) {
        display.direction = Side::RIGHT;
      } else if (display.side == Side::RIGHT) {
        display.direction = Side::LEFT;
      }
    }

    if (display.step == animation.stepsPerRock) {
      display.step = 0;

      if (display.momentum > game.minMomentumToStart) {
        scorePoint();

        if (display.side == Side::LEFT) {
          display.side = Side::RIGHT;
        } else if (display.side == Side::RIGHT) {
          display.side = Side::LEFT;
        }
      }
    }
  } else {
    display.side = Side::CENTER;
    display.direction = Side::CENTER;
    display.rotation = 0;
  }
}

void loop() {
  if (!(arduboy.nextFrame())) {
    return;
  }

  update();
  handleInputs();

  arduboy.clear();

  if (display.showStats) {
    drawStats();
  }

  Xy position = getPosition();
  drawBanana(banana.outerArc, banana.outerRadius, display.rotation, position.x,
             position.y);

  arduboy.drawFastHLine(0, display.floorY, WIDTH);
  drawScore();

  arduboy.display();
}
