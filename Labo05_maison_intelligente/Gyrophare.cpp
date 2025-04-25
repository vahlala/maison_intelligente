#include "Gyrophare.h"

Gyrophare::Gyrophare(int redPin, int greenPin, int bluePin) {
  this->redPin = redPin;
  this->greenPin = greenPin;
  this->bluePin = bluePin;

  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
  eteindre();

  previousMillis = 0;
  currentColor = 0;
  isActive = false;
}

void Gyrophare::allumer() {
  isActive = true;
}

void Gyrophare::eteindre() {
  isActive = false;
  setColor(0, 0, 0);
}

void Gyrophare::setColor(int red, int green, int blue) {
  analogWrite(redPin, red);
  analogWrite(greenPin, green);
  analogWrite(bluePin, blue);
}

void Gyrophare::update() {
  if (!isActive) return;

  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    if (currentColor == 0) {
      setColor(255, 0, 0);
      currentColor = 1;
    } else {
      setColor(0, 0, 255);
      currentColor = 0;
    }
  }
}