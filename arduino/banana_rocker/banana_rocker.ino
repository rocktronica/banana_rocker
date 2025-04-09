#include <Arduboy2.h>
#include <ArduboyTones.h>
#include <Tinyfont.h>

Arduboy2 arduboy;
ArduboyTones sound(arduboy.audio.enabled);
Tinyfont tinyfont = Tinyfont(arduboy.sBuffer, WIDTH, HEIGHT);

// TODO:
// * optimize variable types
// * see about inlining math functions
// * banana maker to find values, test difficulty

const uint16_t SCORE[] PROGMEM = {NOTE_C4, 34, NOTE_E4,  34,
                                  NOTE_C5, 68, TONES_END};
const uint16_t FLIP[] PROGMEM = {NOTE_E3, 34, NOTE_F3, 34, TONES_END};
const uint16_t SLOW[] PROGMEM = {NOTE_E3, 16, NOTE_F3, 34, TONES_END};

// clang-format off
const unsigned char PROGMEM title[] =
{
90, 29,
0xf8, 0xfe, 0xfe, 0xff, 0xff, 0xef, 0xff, 0xff, 0xff, 0xfe, 0x9c, 0x00, 0x00, 0x00, 0xe0, 0xf8, 0xfe, 0xff, 0xff, 0x7f, 0x7f, 0xff, 0xfe, 0xfe, 0xf8, 0xe0, 0x00, 0x00, 0xf0, 0xfe, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0xf8, 0xfc, 0xff, 0xff, 0xff, 0xff, 0xfe, 0x00, 0x00, 0xc0, 0xf0, 0xfc, 0xfe, 0xff, 0xff, 0x7f, 0xff, 0xff, 0xfe, 0xfc, 0xf0, 0xc0, 0x00, 0x00, 0xfc, 0xfe, 0xff, 0xff, 0xff, 0xff, 0xfe, 0xfc, 0xf8, 0xfe, 0xff, 0xff, 0xff, 0xfe, 0xf8, 0x00, 0x00, 0xe0, 0xf8, 0xfe, 0xff, 0xff, 0x7f, 0x7f, 0xff, 0xfe, 0xfe, 0xf8, 0xe0, 0x00, 
0x07, 0x1f, 0x1f, 0x3f, 0x3f, 0x3d, 0xbd, 0xbf, 0xbf, 0x9f, 0x8f, 0x86, 0x00, 0x0f, 0x1f, 0x3f, 0x3f, 0x3f, 0x1f, 0x0f, 0x0f, 0x9f, 0xbf, 0xbf, 0xbf, 0x9f, 0x8f, 0x00, 0x07, 0x1f, 0x3f, 0x3f, 0x3f, 0x1f, 0x03, 0x07, 0x9f, 0x9f, 0xbf, 0xbf, 0xbf, 0x9f, 0x8f, 0x00, 0x00, 0x1f, 0x3f, 0x3f, 0xbf, 0xbf, 0x9f, 0x87, 0x1f, 0x3f, 0x3f, 0xbf, 0xbf, 0x1f, 0x00, 0x00, 0x1f, 0x3f, 0x3f, 0xbf, 0xbf, 0x8f, 0x83, 0x8f, 0x9f, 0xbf, 0xbf, 0x3f, 0x3f, 0x1f, 0x01, 0x00, 0x0f, 0x9f, 0xbf, 0xbf, 0xbf, 0x9f, 0x8f, 0x0f, 0x1f, 0x3f, 0x3f, 0x3f, 0x1f, 0x0f, 
0x00, 0x00, 0x00, 0xf0, 0xfe, 0xff, 0xff, 0xff, 0xff, 0xef, 0xff, 0xff, 0xff, 0xff, 0x3c, 0x00, 0x00, 0xf0, 0xfc, 0xff, 0xff, 0xff, 0xff, 0xbf, 0xbf, 0xff, 0xff, 0xff, 0xff, 0xfc, 0xf0, 0x00, 0xf8, 0xfe, 0xff, 0xff, 0xff, 0xff, 0xff, 0x9f, 0x8f, 0x8f, 0x8f, 0x8f, 0x06, 0x00, 0xfe, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0xfe, 0xff, 0xff, 0xdf, 0x8f, 0x00, 0x00, 0x00, 0xfe, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x6f, 0x0f, 0x07, 0x00, 0x00, 0xfc, 0xfe, 0xff, 0xff, 0xff, 0xef, 0xef, 0xff, 0xff, 0xff, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x01, 0x0f, 0x1f, 0x1f, 0x1f, 0x1f, 0x03, 0x07, 0x0f, 0x1f, 0x1f, 0x1f, 0x0e, 0x00, 0x00, 0x03, 0x0f, 0x0f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x0f, 0x0f, 0x03, 0x00, 0x00, 0x01, 0x03, 0x0f, 0x0f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x0f, 0x07, 0x00, 0x07, 0x0f, 0x1f, 0x1f, 0x1f, 0x0f, 0x07, 0x0f, 0x1f, 0x1f, 0x1f, 0x1f, 0x0e, 0x00, 0x00, 0x07, 0x0f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1e, 0x0e, 0x00, 0x00, 0x07, 0x0f, 0x1f, 0x1f, 0x1f, 0x0f, 0x03, 0x0f, 0x1f, 0x1f, 0x1f, 0x0f, 0x00, 0x00, 0x00, 0x00,
};
// clang-format on

enum Side { CENTER, LEFT, RIGHT, UP, DOWN };
enum GameState { TITLE, ACTIVE, TIPPING, GAME_OVER };

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

  GameState state = GameState::TITLE;
} game;

// Some of these might make more sense in other structs.
// Maybe Input or Game? See what feels weird.
struct Display {
  const int titleSpriteY = 3;
  const int titleSpriteWidth = 90;
  const int textY = 15;
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
  const float initialMomentum = .15;
  const float momentumIncrement = 1.15;

  const int outerRadius = 25;
  const int outerArc = 170;

  const int innerRadius = 50;
  const int innerArc = 56;
  const int depth = 43;
  const int accentRadius = 37;
  const int accentArc = 43;
  const int accentDepth = 25;

  const int stemLength = 10;
  const int stemDepth = 5;
  const int stemOverlap = 10;

  const int tippingAmplitudeDegrees = (170 / 2) + 1;
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
  game.state = GameState::TITLE;
}

// TODO: pause on title on new game bc the title is so great
void resetGame() {
  reset();
  game.scoreDisplayed = 0;
}

void setup() {
  arduboy.beginDoFirst();
  arduboy.waitNoButtons();
  arduboy.setFrameRate(animation.frameRate);

  reset();
}

// NOTE: startingAngle is clockwise from bottom
void drawSemiCircle(int startingAngle, int arc, int radius, uint8_t color,
                    int x, int y) {
  for (int angle = (startingAngle + 90); angle <= (startingAngle + 90) + arc;
       ++angle) {
    float radian = radians(angle);
    int px = x + radius * cos(radian);
    int py = y + radius * sin(radian);

    // HACK!: for math reasons I don't know or care about, the bottom and right
    // pixels both stick out one. This very dumbly nudges them back in.
    if (angle == 0 || angle == 360) {
      px -= 1;
    } else if (angle == 90) {
      py -= 1;
    }

    arduboy.drawPixel(px, py, color);
  }
}

void drawBanana(Banana banana, Display display, Position position) {
  float radian = radians(display.rotation - 90);

  arduboy.fillCircle(position.x, position.y, banana.outerRadius);
  arduboy.fillCircle(position.x + banana.depth * cos(radian),
                     position.y + banana.depth * sin(radian),
                     banana.innerRadius, BLACK);

  //  Inner accent
  drawSemiCircle(display.rotation - banana.accentArc / 2, banana.accentArc,
                 banana.accentRadius, BLACK,
                 position.x + banana.accentDepth * cos(radian),
                 position.y + banana.accentDepth * sin(radian));

  // Stem
  drawSemiCircle(display.rotation + banana.outerArc / 2 - banana.stemOverlap,
                 banana.stemLength, banana.outerRadius, WHITE, position.x,
                 position.y);
  for (int i = 1; i < banana.stemDepth - 1; i++) {
    drawSemiCircle(display.rotation + banana.outerArc / 2 - banana.stemOverlap,
                   banana.stemLength, banana.outerRadius - i, BLACK, position.x,
                   position.y);
    drawSemiCircle(display.rotation + banana.outerArc / 2 - banana.stemOverlap +
                       banana.stemLength,
                   1, banana.outerRadius - i, WHITE, position.x, position.y);
  }
  drawSemiCircle(display.rotation + banana.outerArc / 2 - banana.stemOverlap,
                 banana.stemLength, banana.outerRadius - (banana.stemDepth - 1),
                 WHITE, position.x, position.y);
}

void startNewGame(Side side) {
  game.gamesPlayed += 1;
  game.score = 0;
  game.state = GameState::ACTIVE;

  display.momentum = banana.initialMomentum;
  display.side = side;
}

void endGame() { game.state = GameState::GAME_OVER; }

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

  if (arduboy.justPressed(A_BUTTON) && arduboy.justPressed(B_BUTTON)) {
    display.showStats = !display.showStats;
  }

  if (game.state == GameState::GAME_OVER) {
    if (arduboy.justPressed(A_BUTTON | B_BUTTON | RIGHT_BUTTON | LEFT_BUTTON |
                            DOWN_BUTTON | UP_BUTTON)) {
      resetGame();
    }

    return;
  }

  if (arduboy.anyPressed(RIGHT_BUTTON | LEFT_BUTTON)) {
    if (display.momentum <= game.minMomentumToStart) {
      startNewGame(input.hold);
    } else {
      display.weight = getWeight(input.hold, display.direction, animation.frame,
                                 animation.framesPerRock);
      display.momentum *= max(1, banana.momentumIncrement * display.weight);
    }
  }

  if (display.momentum > game.minMomentumToStart) {
    if (arduboy.anyPressed(DOWN_BUTTON | UP_BUTTON)) {
      sound.tones(SLOW);
      slowDown(animation.momentumDropPerFrame * 2);
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

  if (display.momentum <= game.minMomentumToStart) {
    resetGame();
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

void printCenteredText(__FlashStringHelper *string, int i) {
  tinyfont.setCursor((WIDTH - 5 * 6) / 2, display.textY + 5 * i);
  tinyfont.print(string);
}

void drawText() {
  if (game.state == GameState::TITLE) {
    Sprites::drawSelfMasked((WIDTH - display.titleSpriteWidth) / 2,
                            display.titleSpriteY, title, 0);
  } else if (game.state == GameState::GAME_OVER) {
    printCenteredText(F("GAME"), 0);
    printCenteredText(F("OVER"), 1);
  }
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
                       (banana.outerArc / 2) *
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
      sound.tones(FLIP);
      endGame();
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

  drawBanana(banana, display, getPosition());

  if (display.showStats) {
    drawStats();
  }

  arduboy.drawFastHLine(0, display.floorY, WIDTH);
  drawText();
  drawScore();

  arduboy.display();
}
