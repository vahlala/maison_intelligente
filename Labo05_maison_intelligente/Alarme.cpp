#include "Alarme.h"

Alarme::Alarme(int pin, int high, int low, Gyrophare* gyrophare) {
    this->pin = pin;
    this->high = high;
    this->low = low;
    this->gyrophare = gyrophare; 
    etat = Eteint;
    pinMode(pin, OUTPUT);
    digitalWrite(pin, low);
}

void Alarme::activer() {
    if (etat == Eteint) {
        digitalWrite(pin, high);
        etat = Allumer;
        gyrophare->allumer(); 
    }
}

void Alarme::desactiver() {
    if (etat == Allumer) {
        digitalWrite(pin, low);
        etat = Eteint;
        gyrophare->eteindre(); 
    }
}

void Alarme::update() {
    if (gyrophare) {
        gyrophare->update(); 
    }
}