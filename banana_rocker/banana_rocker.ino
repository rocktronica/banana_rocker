#include <Arduboy2.h>
#include <ArduboyTones.h>
#include <Tinyfont.h>

#include "banana.h"

Arduboy2 arduboy;
ArduboyTones sound(arduboy.audio.enabled);
Tinyfont tinyfont = Tinyfont(arduboy.sBuffer, WIDTH, HEIGHT);

// TODO/EXPLORE:
// * optimize variable types
// * banana maker to find values, test difficulty
// * setGameStage that toggles what gets shown
// * title bounces away against banana?
// * volume control

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
const unsigned char PROGMEM gameover[] =
{
// width, height,
19, 9,
// FRAME 00
0xef, 0x29, 0x29, 0xed, 0x00, 0xef, 0x05, 0x05, 0xef, 0x00, 0xef, 0x61, 0x63, 0x2f, 0x00, 0xef, 0xab, 0xab, 0xe9, 
0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00,
};
// clang-format on

enum Side { CENTER, LEFT, RIGHT, UP, DOWN };
enum GameStage { PREFACE, TITLE, ACTIVE, TIPPING, GAME_OVER };

struct Position {
  int x;
  int y;
};

struct Animation {
  const int frameRate = 30;

  const float momentumDropPerFrame = .05;
  const int framesPerRock = 15;
  int frame = 0;

  const int titleTransitionFrames = 15;
  int titleTransitionFrame = 0;

  const int gameOverTextTransitionFrames = 10;
  int gameOverTextTransitionFrame = 0;
} animation;

struct Game {
  const float minMomentumToStart = .01;
  const float minMomentumToScore = .1;
  const int scoreIncrementMax = 10;

  unsigned int score = 0;
  unsigned int scoreDisplayed = 0;
  unsigned int scoreBest = 0;
  int gamesPlayed = 0;

  GameStage stage = GameStage::TITLE;
} game;

// Some of these might make more sense in other structs.
// Maybe Input or Game? See what feels weird.
struct Display {
  const int titleSpriteWidth = title[0];
  const int titleSpriteHeight = title[1];
  const int gameOverTextWidth = gameover[0];
  const int gameOverTextY = 23;
  const int gameOverTextStartLeft = -gameover[0];
  const int gameOverTextStartRight = WIDTH;
  const int floorY = HEIGHT - 6;
  const int scoreY = HEIGHT - 4;

  Position tippedPosition;

  const int titleTransitionMsMin = 1500;
  int titleTransitionMsStart = 0;
  int titleTransitionMsDisplayed = 0;

  const int titleSpriteYStart = -title[1];
  const int titleSpriteYEnd = 3;
  int titleSpriteY = -title[1];
  bool isTitleActive = true;
  bool isGameOverTextActive = false;

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

Banana banana;

// Leave banana where it is but otherwise reset
void softReset() {
  animation.frame = 0;
  animation.titleTransitionFrame = 0;

  display.titleTransitionMsStart = 0;
  display.titleTransitionMsDisplayed = 0;
  display.controlledRotation = 0;
  display.momentum = 0;
  display.deviation = 0;
  display.weight = 1;
  display.isTitleActive = true;
  display.isGameOverTextActive = false;

  game.score = 0;
  game.stage = GameStage::TITLE;
}

// Reset, for real
void hardReset() {
  softReset();

  display.rotation = 0;
  display.direction = Side::CENTER;
  display.side = Side::CENTER;
}

void hardResetGame() {
  hardReset();
  game.scoreDisplayed = 0;
}

void softResetGame() {
  softReset();
  game.scoreDisplayed = 0;
}

void setup() {
  arduboy.beginDoFirst();
  arduboy.waitNoButtons();
  arduboy.setFrameRate(animation.frameRate);

  hardReset();
  game.stage = GameStage::PREFACE;
}

void drawBanana(Banana banana, Display display, Position position) {
  drawBanana(banana, display.rotation, position.x, position.y);
}

void startNewGame(Side side) {
  game.gamesPlayed += 1;
  game.score = 0;
  game.stage = GameStage::ACTIVE;

  display.isTitleActive = false;
  display.momentum = banana.initialMomentum;
  display.side = side;
}

void endGame() {
  game.stage = GameStage::GAME_OVER;
  display.titleTransitionMsStart = millis();
  display.titleTransitionMsDisplayed = 0;
  display.isGameOverTextActive = true;
}

void handleInputs() {
  if (display.titleTransitionMsDisplayed < display.titleTransitionMsMin) {
    return;
  }

  arduboy.pollButtons();

  if (arduboy.pressed(LEFT_BUTTON) || arduboy.pressed(A_BUTTON)) {
    input.hold = Side::LEFT;
  } else if (arduboy.pressed(RIGHT_BUTTON) || arduboy.pressed(B_BUTTON)) {
    input.hold = Side::RIGHT;
  } else if (arduboy.pressed(UP_BUTTON)) {
    input.hold = Side::UP;
  } else if (arduboy.pressed(DOWN_BUTTON)) {
    input.hold = Side::DOWN;
  } else {
    input.hold = Side::CENTER;
  }

  if (game.stage == GameStage::PREFACE && input.hold != Side::CENTER) {
    hardResetGame();
    return;
  }

  if (game.stage == GameStage::GAME_OVER && input.hold != Side::CENTER) {
    softResetGame();
    return;
  }

  if (input.hold == Side::LEFT || input.hold == Side::RIGHT) {
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

Position getBananaPosition() {
  Position xy, xyEnd;

  xy.x = WIDTH / 2 + (display.rotation / 360.0) * banana.outerRadius * 2 * M_PI;
  xy.y = display.floorY - banana.outerRadius;

  if (game.stage == GameStage::TIPPING || game.stage == GameStage::GAME_OVER ||
      isGameOverToNewGameTransition()) {
    xyEnd.x = xy.x + sin(radians(display.rotation)) * banana.outerRadius +
              (display.direction == Side::RIGHT ? -banana.outerRadius
                                                : banana.outerRadius);
    xyEnd.y =
        xy.y +
        cos(radians(90 - display.rotation)) *
            (banana.outerRadius * (display.direction == Side::RIGHT ? -1 : 1)) +
        banana.outerRadius + 1;

    if (game.stage == GameStage::GAME_OVER) {
      display.tippedPosition = xyEnd;
    }

    if (isGameOverToNewGameTransition()) {
      float weight = easeInSine((float(animation.gameOverTextTransitionFrame) /
                                 animation.gameOverTextTransitionFrames));
      xy.x = WIDTH / 2 + weight * (display.tippedPosition.x - WIDTH / 2);
      xy.y = xy.y + weight * (xyEnd.y - xy.y);

      return xy;
    }

    return xyEnd;
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

// https://github.com/nicolausYes/easing-functions
double easeInSine(double t) { return sin(1.5707963 * t); }

const float getEasedDeviation(Side fromSide, Side toSide, float i, int count) {
  return easeInSine(getLinearDeviation(fromSide, toSide, i, count));
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
    hardResetGame();
  }
}

int getTitleSpriteY() {
  if (game.gamesPlayed == 0) {
    return display.titleSpriteYEnd;
  }

  int travel = display.titleSpriteYEnd - display.titleSpriteYStart;
  float weight = easeInSine((float(animation.titleTransitionFrame) /
                             animation.titleTransitionFrames));

  return display.titleSpriteYStart + (weight * travel);
}

int getGameOverTextX() {
  int start = display.side == Side::LEFT ? display.gameOverTextStartLeft
                                         : display.gameOverTextStartRight;

  if (animation.gameOverTextTransitionFrame == 0) {
    return start;
  }

  int destination =
      min(WIDTH - display.gameOverTextWidth,
          max(0, getBananaPosition().x - display.gameOverTextWidth / 2));
  int travel = destination - start;
  float weight = easeInSine((float(animation.gameOverTextTransitionFrame) /
                             animation.gameOverTextTransitionFrames));

  return start + weight * travel;
}

void drawText() {
  if (display.titleSpriteY > display.titleSpriteYStart) {
    Sprites::drawSelfMasked((WIDTH - display.titleSpriteWidth) / 2,
                            display.titleSpriteY, title, 0);
  }

  int gameOverTextX = getGameOverTextX();
  if (gameOverTextX > display.gameOverTextStartLeft ||
      gameOverTextX < display.gameOverTextStartRight) {
    Sprites::drawSelfMasked(gameOverTextX, display.gameOverTextY, gameover, 0);
  }
}

inline int getTextWidth(int charCount) {
  return charCount * 4 + (charCount - 1) * 1;
}

void drawScore() {
  int scoreBestDigits = 1;
  if (game.scoreBest >= 1000000) {
    scoreBestDigits = 7;
  } else if (game.scoreBest >= 100000) {
    scoreBestDigits = 6;
  } else if (game.scoreBest >= 10000) {
    scoreBestDigits = 5;
  } else if (game.scoreBest >= 1000) {
    scoreBestDigits = 4;
  } else if (game.scoreBest >= 100) {
    scoreBestDigits = 3;
  } else if (game.scoreBest >= 10) {
    scoreBestDigits = 2;
  }

  tinyfont.setCursor(0, display.scoreY);
  tinyfont.print(F("SCORE"));
  tinyfont.setCursor(getTextWidth(5) + 2, display.scoreY);
  tinyfont.print(game.scoreDisplayed);

  tinyfont.setCursor(WIDTH - (getTextWidth(4 + scoreBestDigits) + 1),
                     display.scoreY);
  tinyfont.print(F("BEST"));
  tinyfont.setCursor(WIDTH - getTextWidth(scoreBestDigits), display.scoreY);

  tinyfont.print(game.score == game.scoreBest ? game.scoreDisplayed
                                              : game.scoreBest);
}

bool isGameOverToNewGameTransition() {
  return game.stage == GameStage::TITLE &&
         animation.gameOverTextTransitionFrame > 0;
}

void updateRotation() {
  display.deviation = getEasedDeviation(
      Side::CENTER, display.side, animation.frame, animation.framesPerRock);

  if (isGameOverToNewGameTransition()) {
    display.rotation = (float(animation.gameOverTextTransitionFrame) /
                        animation.gameOverTextTransitionFrames) *
                       180 * (display.side == Side::LEFT ? -1 : 1);
    return;
  }

  if (game.stage == GameStage::TIPPING || game.stage == GameStage::GAME_OVER) {
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

  display.titleSpriteY = getTitleSpriteY();

  if (display.isTitleActive) {
    if (animation.titleTransitionFrame < animation.titleTransitionFrames) {
      animation.titleTransitionFrame += 1;
    }
  } else if (animation.titleTransitionFrame > 0) {
    animation.titleTransitionFrame -= 1;
  }

  if (display.isGameOverTextActive) {
    if (animation.gameOverTextTransitionFrame <
        animation.gameOverTextTransitionFrames) {
      animation.gameOverTextTransitionFrame += 1;
    }
  } else if (animation.gameOverTextTransitionFrame > 0) {
    animation.gameOverTextTransitionFrame -= 1;
  }

  if (game.stage == GameStage::PREFACE || game.stage == GameStage::TITLE ||
      game.stage == GameStage::GAME_OVER) {
    if (display.titleTransitionMsDisplayed < display.titleTransitionMsMin) {
      if (display.titleTransitionMsStart == 0) {
        display.titleTransitionMsStart = millis();
        return;
      }

      display.titleTransitionMsDisplayed =
          millis() - display.titleTransitionMsStart;
    }
  }

  if (game.stage == GameStage::PREFACE &&
      display.titleTransitionMsDisplayed >= display.titleTransitionMsMin) {
    hardReset();
  }

  if (game.stage == GameStage::PREFACE || game.stage == GameStage::TITLE ||
      game.stage == GameStage::GAME_OVER || isGameOverToNewGameTransition()) {
    return;
  }

  if (abs(display.rotation) >= banana.tippingAmplitudeDegrees) {
    if (game.stage == GameStage::ACTIVE) {
      // No matter where we are in the current rocking swing, if
      // we're about to tip, reset current frame to give the falling
      // animation a regular half cycle to complete.
      animation.frame = ceil(animation.framesPerRock / 2.0);
    }
    game.stage = GameStage::TIPPING;
    display.direction = display.side;
  }

  if (game.stage == GameStage::TIPPING) {
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

void drawPreface() {
  int y = (HEIGHT - 7 * 5) / 2;

  arduboy.clear();

  tinyfont.setCursor((WIDTH - getTextWidth(7)) / 2, y + 5 * 0);
  tinyfont.print(F("MADE BY"));
  tinyfont.setCursor((WIDTH - getTextWidth(11)) / 2, y + 5 * 1);
  tinyfont.print(F("ROCKTRONICA"));
  tinyfont.setCursor((WIDTH - getTextWidth(7)) / 2, y + 5 * 3);
  tinyfont.print(F("FOR THE"));
  tinyfont.setCursor((WIDTH - getTextWidth(7)) / 2, y + 5 * 4);
  tinyfont.print(F("ARDUBOY"));
  tinyfont.setCursor((WIDTH - getTextWidth(10)) / 2, y + 5 * 5);
  tinyfont.print(F("BANANA JAM"));
  tinyfont.setCursor((WIDTH - getTextWidth(4)) / 2, y + 5 * 7);
  tinyfont.print(F("2025"));
  arduboy.display();
}

void drawGame() {
  arduboy.clear();

  drawBanana(banana, display, getBananaPosition());

  arduboy.drawFastHLine(0, display.floorY, WIDTH);
  drawText();
  drawScore();

  arduboy.display();
}

void loop() {
  if (!(arduboy.nextFrame())) {
    return;
  }

  update();
  handleInputs();

  if (game.stage == GameStage::PREFACE) {
    drawPreface();
  } else {
    drawGame();
  }
}
