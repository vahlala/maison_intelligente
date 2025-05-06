#include "PorteAutomatique.h"
#include <Arduino.h>
#include <AccelStepper.h>

#define MOTOR_INTERFACE_TYPE 4

PorteAutomatique::PorteAutomatique(int p1, int p2, int p3, int p4, float& distancePtr)
    : _stepper(MOTOR_INTERFACE_TYPE, p1, p2, p3, p4), _distance(distancePtr) {
      _stepper.enableOutputs();
    _stepper.setMaxSpeed(800);
    _stepper.setAcceleration(120);
    _stepper.setSpeed(200);
    _stepper.moveTo(-_angleEnSteps(_angleOuvert));
}

void PorteAutomatique::update() {
    _mettreAJourEtat();
    _stepper.run();
}

void PorteAutomatique::setAngleOuvert(float angle) {
    _angleOuvert = angle;
}

void PorteAutomatique::setAngleFerme(float angle) {
    _angleFerme = angle;
}

void PorteAutomatique::setPasParTour(int steps) {
    _stepsPerRev = steps;
}

void PorteAutomatique::setDistanceOuverture(float distance) {
    _distanceOuverture = distance;
}

void PorteAutomatique::setDistanceFermeture(float distance) {
    _distanceFermeture = distance;
}

const char* PorteAutomatique::getEtatTexte() const {
    switch (_etat) {
    case FERMEE: return "FERMEE";
    case OUVERTE: return "OUVERTE";
    case EN_FERMETURE: return "EN FERMETURE";
    case EN_OUVERTURE: return "EN OUVERTURE";
    default: return "INCONNU";
    }
}

float PorteAutomatique::getAngle() const {
    return _angleEnSteps(_stepper.currentPosition());
}

void PorteAutomatique::_mettreAJourEtat() {
    if (_distance < _distanceOuverture) {
        if (_etat != EN_OUVERTURE && _etat != OUVERTE) {
            _ouvrir();
        }
    }
    else if (_distance > _distanceFermeture) {
        if (_etat != EN_FERMETURE && _etat != FERMEE) {
            _fermer();
        }
    }
}

void PorteAutomatique::_ouvertureState() {
    if (_stepper.currentPosition() < _angleEnSteps(_angleOuvert)) {
        _stepper.enableOutputs();
        _stepper.moveTo(_angleEnSteps(_angleOuvert));
    }
}

void PorteAutomatique::_fermetureState() {
  if (_stepper.distanceToGo() == 0) {
    if ((_stepper.currentPosition() >= _angleEnSteps(_angleOuvert))&& (_stepper.currentPosition() != _angleEnSteps(_angleFerme))) {
         _stepper.enableOutputs();
        _stepper.moveTo(- _stepper.currentPosition());
    }
  } 
}

void PorteAutomatique::_ouvertState() {
    _stepper.stop();
    _stepper.disableOutputs();
}

void PorteAutomatique::_fermeState() {
    _stepper.stop();
    _stepper.disableOutputs();
}

void PorteAutomatique::_ouvrir() {
    _etat = EN_OUVERTURE;
    _ouvertureState();
}

void PorteAutomatique::_fermer() {
    _etat = EN_FERMETURE;
    _fermetureState();
}


long PorteAutomatique::_angleEnSteps(float angle) const {
    return (angle / 360.0) * _stepsPerRev;
}
