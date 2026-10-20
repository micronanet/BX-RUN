// BX RUN: Arduino entry points must keep external linkage.
#include "jogo.h"

void setup();
void loop();

void setup() {
  setupGame();
}

void loop() {
  loopGame();
}
