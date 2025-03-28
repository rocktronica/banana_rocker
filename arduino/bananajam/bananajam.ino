#include <Arduboy2.h>

Arduboy2 arduboy;

void setup() {
  arduboy.boot();
  arduboy.setFrameRate(15);
}

void loop() {
  if (!(arduboy.nextFrame())) {
    return;
  }

  arduboy.clear();

  arduboy.setCursor(4, 9);
  arduboy.print(F("Hello, world!"));

  arduboy.display();
}
