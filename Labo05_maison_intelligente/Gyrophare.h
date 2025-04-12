#pragma once
#include <Arduino.h>

class Gyrophare {
public:
    Gyrophare(int redPin, int greenPin, int bluePin);

    void allumer();
    void eteindre();
    void setColor(int red, int green, int blue);
    void update(); 

private:
    int redPin;
    int greenPin;
    int bluePin;

    unsigned long previousMillis;
    const long interval = 125; 
    int currentColor; 
    bool isActive; 
};