#pragma once
#include <string.h>
#include <arduino.h>
#include <U8g2lib.h>


class MaisonIntelligente {
public:
  MaisonIntelligente();
  MaisonIntelligente(float* currentDistance);
  void initDisplay(U8G2_MAX7219_8X8_F_4W_SW_SPI* display);

  void setMinDist(int dist);
  int getMinDist() const;

  void setMaxDist(int dist);
  int getMaxDist() const;

  void setAlarmLimit(int dist);
  int getAlarmLimit() const;

  void SerialCommand();

  void updateDisplay();

private:
  int zero = 0;
  int contrast = 200;
  long interval = 3000;
  float* Distance;
  String command;
  int minDist = 30;
  int maxDist = 60;
  int alarmLimit;
  bool cmdReceived;
  void traiterCommande(const String& commande);
  void decomposerCommande(const String& commande, String& option, String& action, String& valeur);
  void executerAction(const String& option, const String& action, const String& valeur);
  unsigned long displayStartTime = 0;
  bool isDisplaying = false;
  enum DisplayState { NONE,
                      CHECK,
                      ERROR,
                      CROSS } displayState = NONE;
  U8G2_MAX7219_8X8_F_4W_SW_SPI* u8g2;
  void afficherCheck();
  void afficherErreur();
  void afficherCross();
  bool isNumber(const String& str);
};
