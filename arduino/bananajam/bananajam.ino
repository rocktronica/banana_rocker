#include <Arduboy2.h>

Arduboy2 arduboy;

// TODO:
// * optimize variable types
// * deal w/ 1+ momentum

const int frameRate = 24;

const int rockingAmplitudeDegrees = 90;
const int rockingFrequencyMs = 1500;

const int stepsPerRock = frameRate * (rockingFrequencyMs / 1000.0);
const float momentumDropPerStep = .0125;

int radius = 25;
int coverage = 170;
int rotation = 0;
int step = 0;
float momentum = 0;
int direction = 0; // -1 = left, 1 = right

bool showStats = true;

double easeInOutSine(double t) {
  return 0.5 * (1 + sin(3.1415926 * (t - 0.5)));
}

void setup() {
  arduboy.boot();
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

void handleInputs() {
  arduboy.pollButtons();

  if (arduboy.pressed(LEFT_BUTTON)) {
    if (direction == 1) {
      momentum -= .1;
    } else {
      momentum += .1;
    }
  } else if (arduboy.pressed(RIGHT_BUTTON)) {
    if (direction == -1) {
      momentum -= .1;
    } else {
      momentum += .1;
    }
  }

  if (arduboy.justPressed(A_BUTTON)) {
    showStats = !showStats;
  }
}

inline int getX() { return (rotation / 360.0) * radius * 2 * M_PI; }

void loop() {
  if (!(arduboy.nextFrame())) {
    return;
  }

  float linearDeviation = 0;
  float easedDeviation = 0;

  if (momentum > 0) {
    step = (step + 1) % stepsPerRock;
    momentum = max(0, momentum - momentumDropPerStep);

    linearDeviation = abs((float(step) / stepsPerRock) - 1.0 / 2) * 2;
    easedDeviation = easeInOutSine(linearDeviation);

    if (linearDeviation == .5) {
      direction = 0;
    } else {
      // TODO: fix this. it's not rights
      direction = linearDeviation < .5 ? -1 : 1;
    }

    rotation = momentum * (easedDeviation * rockingAmplitudeDegrees * 2 -
                           rockingAmplitudeDegrees);
  } else {
    step = 0;
    direction = 0;
  }

  handleInputs();

  arduboy.clear();

  if (showStats) {
    arduboy.setCursor(0, 0);
    arduboy.print(linearDeviation);
    arduboy.setCursor(30, 0);
    arduboy.print(rotation);
    arduboy.setCursor(60, 0);
    arduboy.print(direction);
    arduboy.setCursor(90, 0);
    arduboy.print(momentum);
  }

  drawBanana(coverage, radius, rotation, center.x + getX(), center.y);
  arduboy.drawFastHLine(0, floorY, WIDTH);

  arduboy.display();
}
