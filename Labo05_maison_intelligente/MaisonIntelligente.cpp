#include "MaisonIntelligente.h"
#include <U8g2lib.h>


MaisonIntelligente::MaisonIntelligente(float* currentDistance) {
  Distance = currentDistance;
}


void MaisonIntelligente::setMaxDist(int dist) {
  maxDist = dist;
}
int MaisonIntelligente::getMaxDist() const {
  return maxDist;
}

void MaisonIntelligente::setAlarmLimit(int dist) {
  alarmLimit = dist;
}
int MaisonIntelligente::getAlarmLimit() const {
  return alarmLimit;
}

void MaisonIntelligente::setMinDist(int dist) {
  minDist = dist;
}
int MaisonIntelligente::getMinDist() const {
  return minDist;
}

void MaisonIntelligente::SerialCommand() {
  if (Serial.available()) {
    command = Serial.readStringUntil('\n');
    command.trim();
    if (command.length() > zero) {
      traiterCommande(command);
    }
  }
}
bool MaisonIntelligente::isNumber(const String& str) {
  if (str.length() == zero) return false;

  int startIndex = zero;
  if (str.charAt(zero) == '-' || str.charAt(zero) == '+') {
    if (str.length() == 1) return false;
  }

  for (int i = startIndex; i < str.length(); i++) {
    if (!isDigit(str.charAt(i))) {
      return false;
    }
  }

  return true;
}



void MaisonIntelligente::traiterCommande(const String& commande) {
  if (commande == "g_dist") {
    Serial.println(*Distance);
    afficherCheck();
    return;
  }

  if (commande.startsWith("cfg;")) {
    String option, action, valeur;
    decomposerCommande(commande, option, action, valeur);
    executerAction(option, action, valeur);
  } else {
    Serial.println("Commande inconnue!");
    afficherCross();
  }
}

void MaisonIntelligente::decomposerCommande(const String& commande, String& option, String& action, String& valeur) {
  int i1 = commande.indexOf(';');
  int i2 = commande.indexOf(';', i1 + 1);
  int i3 = commande.indexOf(';', i2 + 1);

  option = commande.substring(0, i1);
  action = commande.substring(i1 + 1, i2);
  valeur = commande.substring(i2 + 1);
}


void MaisonIntelligente::executerAction(const String& option, const String& action, const String& valeur) {
  if (!isNumber(valeur)) {
    afficherCross();
    return;
  }

  int val = valeur.toInt();


  if (action == "alm") {
    afficherCheck();
    setAlarmLimit(val);
    Serial.print("Configure la distance de détection de l’alarme à : ");
    Serial.print(val);
    Serial.println(" cm");

  } else if (action == "lim_inf") {
    if (val < maxDist) {
      afficherCheck();
      setMinDist(val);
      Serial.print("Il configure sa limite inférieure du moteur a : ");
      Serial.println(val);
    } else {
      afficherErreur();
      Serial.println("Erreur : Limite inférieure >= limite supérieure.");
    }

  } else if (action == "lim_sup") {
    if (val > minDist) {
      afficherCheck();
      setMaxDist(val);
      Serial.print("Il configure sa Limite supérieure du moteur a : ");
      Serial.println(val);
    } else {
      afficherErreur();
      Serial.println("Erreur : Limite supérieure <= limite inférieure.");
    }

  } else {
    afficherErreur();
    Serial.println("Action inconnue !");
  }
}

void MaisonIntelligente::initDisplay(U8G2_MAX7219_8X8_F_4W_SW_SPI* display) {
  u8g2 = display;
  u8g2->begin();
  u8g2->setContrast(contrast);
}
void MaisonIntelligente::afficherCheck() {
  u8g2->clearBuffer();
  u8g2->drawLine(1, 5, 3, 7);
  u8g2->drawLine(3, 7, 7, 1);
  u8g2->sendBuffer();

  displayStartTime = millis();
  displayState = CHECK;
  isDisplaying = true;
}

void MaisonIntelligente::afficherErreur() {
  u8g2->clearBuffer();
  u8g2->drawCircle(3, 3, 3);
  u8g2->drawLine(0, 0, 7, 7);
  u8g2->sendBuffer();

  displayStartTime = millis();
  displayState = ERROR;
  isDisplaying = true;
}
void MaisonIntelligente::afficherCross() {
  u8g2->clearBuffer();
  u8g2->drawLine(7, 7, 0, 0);
  u8g2->drawLine(0, 7, 7, 0);
  u8g2->sendBuffer();

  displayStartTime = millis();
  displayState = CROSS;
  isDisplaying = true;
}

void MaisonIntelligente::updateDisplay() {
  if (isDisplaying && (millis() - displayStartTime >= interval)) {
    u8g2->clear();
    isDisplaying = false;
    displayState = NONE;
  }
}
