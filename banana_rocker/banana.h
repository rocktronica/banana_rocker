struct Banana {
  // NOTE: unapologetically magic values here, who cares
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
};

void drawSemiCircle(int startingAngle, int arc, int radius, uint8_t color,
                    int x, int y);

void drawBanana(Banana banana, int rotation, int x, int y);