#include <AccelStepper.h>
#include <HCSR04.h>
#include "Alarme.h"
#include "Gyrophare.h"
#include <LCD_I2C.h>
#include <Wire.h>
#include "MaisonIntelligente.h"
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

HCSR04 hc(TRIGGER_PIN, ECHO_PIN);
LCD_I2C lcd(0x27, 16, 2);
AccelStepper myStepper(MOTOR_INTERFACE_TYPE, IN_1, IN_3, IN_2, IN_4);
Gyrophare gyrophare(RED_PIN, GREEN_PIN, BLUE_PIN);
Alarme alarme(BUZZER_PIN, HIGH, LOW, &gyrophare);


U8G2_MAX7219_8X8_F_4W_SW_SPI u8g2(
  U8G2_R0,             // rotation
  /* clock=*/CLK_PIN,  // pin Arduino reliée à CLK (horloge)
  /* data=*/DIN_PIN,   // pin Arduino reliée à DIN (données)
  /* cs=*/CS_PIN,      // pin Arduino reliée à CS (chip select)
  /* dc=*/U8X8_PIN_NONE,
  /* reset=*/U8X8_PIN_NONE);

enum SystemState { DEMARRAGE,
                   MESURE_DISTANCE,
                   AFFICHAGE };
SystemState currentState = DEMARRAGE;
float currentDistance = 0.0;
MaisonIntelligente maison(&currentDistance);
const int maxDegreeA = 360;
const int lowestValue = 1;
const int maxValue = 2038.0;
const int maxDegreeB = 170.0;
const int minDegreeB = 10.0;
float MaxStep = maxDegreeB * (maxValue / maxDegreeA);
float MinStep = 0.0;
int initialPos = 0;
int currentDegree = 10;
bool ouvrir = false;
static bool active = false;
static unsigned long lastAlarmeTime = 0;
static unsigned long lastDisplayTime = 0;
static unsigned long lastDistanceTime = 0;
const unsigned long DISPLAY_INTERVAL = 100;
const unsigned long DISTANCE_INTERVAL = 50;
unsigned long currentTime;
float MinValidDist = 30.0;
float MaxValidDist = 60.0;
float MinAlarmeDist = 15.0;
int alarmeDist;
const unsigned long AlarmeInt = 3000;
unsigned long DisableTime = 2000;
static unsigned long disableTime = 0;
int zero = 0;
void setup() {
  maison.initDisplay(&u8g2);
  Serial.begin(115200);
  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);
  lcd.begin();
  lcd.backlight();
  myStepper.enableOutputs();
  myStepper.setMaxSpeed(800);
  myStepper.setAcceleration(120);
  myStepper.setSpeed(200);
  myStepper.moveTo(MinStep);
  displayStartup();
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
  if (distance > 0) {
    currentDistance = distance;
    if (currentDistance < MinValidDist && ouvrir == false) {
      Serial.print("jouvre la porte");
      ouvrir = true;
      controlDoor();
    }
  }
}
void callAlarme() {
  if (currentDistance < MinAlarmeDist && !active) {
    active = true;
    alarme.activer();
  } else {
    if (currentTime - lastAlarmeTime >= AlarmeInt && active) {
      active = false;
      lastAlarmeTime = currentTime;
      alarme.desactiver();
    }
  }
}

void controlDoor() {
  if (ouvrir && myStepper.currentPosition() < MaxStep) {
    myStepper.enableOutputs();
    myStepper.moveTo(MaxStep);
  } else if (!ouvrir && myStepper.currentPosition() > MinStep) {
    myStepper.enableOutputs();
    myStepper.moveTo(MinStep);
  }
  currentDegree = stepsToDegree(myStepper.currentPosition());
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
    lcd.print("Porte: ");
    lcd.print("Fermer");
  } else if (currentDistance < MinValidDist) {
    if (myStepper.currentPosition() == MaxStep) {
      lcd.print("Porte: ");
      lcd.print("Ouverte");
    } else {
      lcd.print("deg:");
      lcd.print(currentDegree);
      lcd.print(" degree");
    }
  }
}


void updateCommandInput() {
  MinValidDist = maison.getMinDist();
  MaxValidDist = maison.getMaxDist();
  alarmeDist = maison.getAlarmLimit();
  if (alarmeDist > zero) {
    MinAlarmeDist = alarmeDist;
  } else {
    MinAlarmeDist = 15.0;
  }
}


void loop() {
  maison.SerialCommand();
  maison.updateDisplay();
  updateCommandInput();
  currentTime = millis();

  if (myStepper.distanceToGo() == zero) {
    if ((currentDistance >= MaxValidDist) && (myStepper.currentPosition() >= MaxStep)) {
      myStepper.moveTo(-myStepper.currentPosition());
    }
    //static unsigned long disableTime = 0;
    if (currentTime - disableTime > DisableTime) {
      myStepper.disableOutputs();
    }
  } else {
    disableTime = currentTime;
  }
  myStepper.run();

  switch (currentState) {
    case DEMARRAGE:
      if (currentTime - lastDisplayTime >= DisableTime) {
        currentState = MESURE_DISTANCE;
        lcd.clear();
      }
      break;

    case MESURE_DISTANCE:
      if (currentTime - lastDistanceTime >= DISTANCE_INTERVAL) {
        lastDistanceTime = currentTime;
        measureDistance();
        currentState = AFFICHAGE;
      }
      break;

    case AFFICHAGE:
      if (currentTime - lastDisplayTime >= DISPLAY_INTERVAL) {
        lcdDisplay();
        lastDisplayTime = currentTime;
      }
      currentState = MESURE_DISTANCE;
      break;
  }
  callAlarme();
  alarme.update();
}

float stepsToDegree(float steps) {
  return ((steps - MinStep) / (MaxStep - MinStep)) * (maxDegreeB - minDegreeB) + minDegreeB;
}