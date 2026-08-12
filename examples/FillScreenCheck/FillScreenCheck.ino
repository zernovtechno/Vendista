#include "Vendista.h"

Vendista myVend(Serial, 500);

void setup() {
  delay(2000);
  Serial.begin(115200);
}

void loop() {
  myVend.FillScreen(VENDISTA_RED_COLOR);
  delay(1000);
  myVend.FillScreen(VENDISTA_GREEN_COLOR);
  delay(1000);
  myVend.FillScreen(VENDISTA_BLUE_COLOR);
  delay(1000);
  myVend.FillScreen(VENDISTA_YELLOW_COLOR);
  delay(1000);
  myVend.FillScreen(VENDISTA_PURPLE_COLOR);
  delay(1000);
  myVend.FillScreen(VENDISTA_CYAN_COLOR);
  delay(1000);
  myVend.FillScreen(VENDISTA_BLACK_COLOR);
  delay(1000);
  myVend.FillScreen(VENDISTA_CYAN_COLOR);
  delay(1000);
}
