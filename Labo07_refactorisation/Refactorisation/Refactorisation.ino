#include <AccelStepper.h>
#include <HCSR04.h>
#include "PorteAutomatique.h"
#include "Alarm.h"
#include "MaisonIntelligente.h"
#include <LCD_I2C.h>
#include <Wire.h>
#include <U8g2lib.h>

#define RED_PIN 35
#define GREEN_PIN 33
#define BLUE_PIN 31
#define TRIGGER_PIN 7
#define BUZZER_PIN 4
#define ECHO_PIN 6
#define MOTOR_INTERFACE_TYPE 4
#define IN_1 9
#define IN_2 10
#define IN_3 11
#define IN_4 12
#define CLK_PIN 30
#define DIN_PIN 34
#define CS_PIN 32

// Initialisation des objets
float currentDistance = 0.0;
HCSR04 hc(TRIGGER_PIN, ECHO_PIN);
LCD_I2C lcd(0x27, 16, 2);
Alarm alarm1(RED_PIN, GREEN_PIN, BLUE_PIN,MOTOR_INTERFACE_TYPE, currentDistance);
PorteAutomatique porte(IN_1, IN_3, IN_2, IN_4, currentDistance);
MaisonIntelligente maison(&currentDistance);
U8G2_MAX7219_8X8_F_4W_SW_SPI u8g2(U8G2_R0, CLK_PIN, DIN_PIN, CS_PIN, U8X8_PIN_NONE, U8X8_PIN_NONE);



enum SystemState { DEMARRAGE,
                   MESURE_DISTANCE,
                   AFFICHAGE };
SystemState currentState = DEMARRAGE;

unsigned long currentTime;
const unsigned long DISPLAY_INTERVAL = 100;
const unsigned long DISTANCE_INTERVAL = 50;
static unsigned long lastDisplayTime = 0;
static unsigned long lastDistanceTime = 0;
float alarmLimit = 0.0;
float MinValidDist = 30.0;
float MaxValidDist = 60.0;
float thresholdDist = 0.01;

void setup() {
  maison.initDisplay(&u8g2);
  Serial.begin(115200);
  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);
  lcd.begin();
  lcd.backlight();
  displayStartup();
  alarm1.setColourA(255, 0, 0);
  alarm1.setColourB(0, 0, 255);
  alarm1.setVariationTiming(200);
  alarm1.setTimeout(3000);
  alarm1.turnOn();
}

void displayStartup() {
  lcd.setCursor(0, 0);
  lcd.print("Labo 4A");
  lcd.setCursor(0, 1);
  lcd.print("etd:2412876");
  lastDisplayTime = millis();
}

void measureDistance() {
  float distance = hc.dist();
  if (distance > thresholdDist) {
    currentDistance = distance;
    porte.update();
  }
}

void lcdDisplay() {
  static String lastLine = "";
  String currentLine = "Dist: " + String(currentDistance) + " cm";
  if (currentLine != lastLine) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(currentLine);
    lastLine = currentLine;
  }
  lcd.setCursor(0, 1);

  if (currentDistance > MinValidDist) {
    lcd.print("Porte: Fermer");
  } else {
    lcd.print("Porte: Ouvrir");
  }
}

void updateCommandInput() {
  MinValidDist = maison.getMinDist();
  porte.setDistanceOuverture(MinValidDist);
  MaxValidDist = maison.getMaxDist();
  porte.setDistanceFermeture(MaxValidDist);

  alarmLimit = maison.getAlarmLimit();
  if (alarmLimit > 0.0){
  alarm1.setDistance(alarmLimit);
  }
}

void loop() {
  maison.SerialCommand();
  maison.updateDisplay();
  updateCommandInput();
  porte.update();
  currentTime = millis();
  if (currentTime - lastDistanceTime >= DISTANCE_INTERVAL) {
    measureDistance();
  }

  if (currentTime - lastDisplayTime >= DISPLAY_INTERVAL) {
    lcdDisplay();
  }
  alarm1.update();
}
