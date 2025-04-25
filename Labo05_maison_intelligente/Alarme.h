#pragma once
#include <Arduino.h>
#include "Gyrophare.h"

enum EtatAlarme {
  Eteint,
  Allumer
};

class Alarme {
public:
  Alarme(int pin, int high, int low, Gyrophare* gyrophare);

  void activer();
  void desactiver();
  void update();

  EtatAlarme getEtat() {
    return etat;
  }

private:
  int pin;
  int high;
  int low;
  EtatAlarme etat;
  Gyrophare* gyrophare;
};