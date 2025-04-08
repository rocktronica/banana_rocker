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
enum GameState { IDLE, ACTIVE, TIPPING, GAME_OVER };

struct Position {
  int x;
  int y;
};

struct Animation {
  const int frameRate = 30;
  const float momentumDropPerFrame = .05;
  const int framesPerRock = 15;

  int frame = 0;
} animation;

struct Game {
  const float minMomentumToStart = .01;
  const float minMomentumToScore = .1;
  const int scoreIncrementMax = 10;

  int score = 0;
  int scoreDisplayed = 0;
  int scoreBest = 0;
  int gamesPlayed = 0;

  GameState state = GameState::IDLE;
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
  animation.frame = 0;

  display.controlledRotation = 0;
  display.rotation = 0;
  display.momentum = 0;
  display.deviation = 0;
  display.direction = Side::CENTER;
  display.side = Side::CENTER;
  display.weight = 1;

  game.score = 0;
  game.state = GameState::IDLE;
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

// TODO: a little stem should be easy enough
void drawBanana(Banana banana, Display display, Position position) {
  float radian = radians(display.rotation - 90);

  drawSemiCircle(display.rotation - banana.outerArc / 2, banana.outerArc,
                 banana.outerRadius, position.x, position.y);
  drawSemiCircle(display.rotation - banana.innerArc / 2, banana.innerArc,
                 banana.innerRadius, position.x + banana.depth * cos(radian),
                 position.y + banana.depth * sin(radian));

  drawSemiCircle(display.rotation - banana.accentArc / 2, banana.accentArc,
                 banana.accentRadius,
                 position.x + banana.accentDepth * cos(radian),
                 position.y + banana.accentDepth * sin(radian));
}

void startMomentum(Side side) {
  game.gamesPlayed += 1;
  game.score = 0;
  game.state = GameState::ACTIVE;

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

  if (game.state == GameState::GAME_OVER) {
    if (arduboy.justPressed(A_BUTTON | B_BUTTON | RIGHT_BUTTON | LEFT_BUTTON |
                            DOWN_BUTTON | UP_BUTTON)) {
      reset();
      game.scoreDisplayed = 0;
    }

    return;
  }

  // TODO: use input.hold
  if (arduboy.anyPressed(RIGHT_BUTTON | LEFT_BUTTON)) {
    // TODO: try minMomentumToScore so it's easier to start again
    if (display.momentum <= game.minMomentumToStart) {
      startMomentum(input.hold);
    } else {
      // TODO: consider updateMomentum()
      display.weight = getWeight(input.hold, display.direction, animation.frame,
                                 animation.framesPerRock);
      display.momentum *= max(1, banana.momentumIncrement * display.weight);
    }
  }

  if (display.momentum > game.minMomentumToStart) {
    if (arduboy.anyPressed(DOWN_BUTTON | UP_BUTTON)) {
      if (arduboy.pressed(DOWN_BUTTON)) {
        sound.tones(SLOW);
      }

      slowDown(animation.momentumDropPerFrame * 4);

      // TODO: (game.state == GameState::ACTIVE)
      // or ditch entirely?
      if (game.state != GameState::TIPPING) {
        game.score = max(0, game.score - 1);
      }
    }
  }
}

Position getPosition() {
  Position xy;

  xy.x = WIDTH / 2 + (display.rotation / 360.0) * banana.outerRadius * 2 * M_PI;
  xy.y = display.floorY - banana.outerRadius;

  if (game.state == GameState::TIPPING || game.state == GameState::GAME_OVER) {
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

  // TODO: reset
  if (display.momentum <= game.minMomentumToStart) {
    display.momentum = 0;
    animation.frame = 0;
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
  tinyfont.print(F("FRAME:"));
  tinyfont.setCursor(x, 5 * line);
  tinyfont.print(animation.frame);
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
  tinyfont.print(F("STATE:"));
  tinyfont.setCursor(x, 5 * line);
  tinyfont.print(game.state);
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
  display.deviation = getEasedDeviation(
      Side::CENTER, display.side, animation.frame, animation.framesPerRock);

  if (game.state == GameState::TIPPING || game.state == GameState::GAME_OVER) {
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

  if (game.state == GameState::GAME_OVER) {
    return;
  }

  if (abs(display.rotation) >= banana.tippingAmplitudeDegrees) {
    if (game.state == GameState::ACTIVE) {
      // No matter where we are in the current rocking swing, if
      // we're about to tip, reset current frame to give the falling
      // animation a regular half cycle to complete.
      animation.frame = ceil(animation.framesPerRock / 2.0);
    }
    game.state = GameState::TIPPING;
    display.direction = display.side;
  }

  if (game.state == GameState::TIPPING) {
    if (animation.frame < animation.framesPerRock) {
      animation.frame += 1;
    } else {
      sound.tones(BUMP);
      game.state = GameState::GAME_OVER;
    }

    return;
  }

  if (display.momentum > 0) {
    animation.frame += 1;
    slowDown(animation.momentumDropPerFrame);

    if (animation.frame == animation.framesPerRock / 2) {
      if (display.side == Side::LEFT) {
        display.direction = Side::RIGHT;
      } else if (display.side == Side::RIGHT) {
        display.direction = Side::LEFT;
      }
    }

    if (animation.frame == animation.framesPerRock) {
      animation.frame = 0;

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

  drawBanana(banana, display, getPosition());

  arduboy.drawFastHLine(0, display.floorY, WIDTH);
  drawScore();

  arduboy.display();
}
