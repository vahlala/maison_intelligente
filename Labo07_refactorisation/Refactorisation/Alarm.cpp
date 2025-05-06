#include "Alarm.h"
#include <Arduino.h>

Alarm::Alarm(int rPin, int gPin, int bPin, int buzzerPin, float& distancePtr)
  : _rPin(rPin), _gPin(gPin), _bPin(bPin), _buzzerPin(buzzerPin), _distance(&distancePtr) {
  pinMode(_rPin, OUTPUT);
  pinMode(_gPin, OUTPUT);
  pinMode(_bPin, OUTPUT);
  pinMode(_buzzerPin, OUTPUT);
  _turnOff();
}

void Alarm::setColourA(int r, int g, int b) {
  _colA[0] = r;
  _colA[1] = g;
  _colA[2] = b;
}

void Alarm::setColourB(int r, int g, int b) {
  _colB[0] = r;
  _colB[1] = g;
  _colB[2] = b;
}

void Alarm::setVariationTiming(unsigned long seconds) {
  _variationRate = seconds;
}

void Alarm::setDistance(float dist) {
  _distanceTrigger = dist;
}

void Alarm::setTimeout(unsigned long seconds) {
  _timeoutDelay = seconds;
}

void Alarm::turnOn() {
  _turnOnFlag = true;
}

void Alarm::turnOff() {
  _turnOffFlag = true;
}

void Alarm::test() {
  _state = TESTING;
  _testStartTime = millis();
  _lastUpdate = millis();  
  _currentColor = false;
}

AlarmState Alarm::getState() const {
  return _state;
}

void Alarm::update() {
  switch (_state) {
    case OFF: _offState(); break;
    case WATCHING: _watchState(); break;
    case ON: _onState(); break;
    case TESTING: _testingState(); break;
  }
}

void Alarm::_setRGB(int r, int g, int b) {
  analogWrite(_rPin, r);
  analogWrite(_gPin, g);
  analogWrite(_bPin, b);
}

void Alarm::_turnOff() {
  digitalWrite(_buzzerPin, LOW);
  _setRGB(0, 0, 0);
}

void Alarm::_offState() {
  _turnOff();
  if (_turnOnFlag) {
    _turnOnFlag = false;
    _state = WATCHING;
  }
}

void Alarm::_watchState() {
  if (_turnOffFlag) {
    _turnOffFlag = false;
    _state = OFF;
    return;
  }

  if (_distance && (*_distance <= _distanceTrigger)) {
  
    _state = ON;
    _lastDetectedTime = millis();
    _lastUpdate = millis();
    _currentColor = false;
    
  }
}

void Alarm::_onState() {
  if (_turnOffFlag) {
    _turnOffFlag = false;
    _state = OFF;
    return;
  }
   
  if (_distance && (*_distance > _distanceTrigger)) {
    if (millis() - _lastDetectedTime > _timeoutDelay) {
      _state = WATCHING;
      _turnOff();
      return;
    }
  } else {
    _lastDetectedTime = millis();  
  }
  
  digitalWrite(_buzzerPin, HIGH);

  unsigned long ttime = millis();
  if (ttime - _lastUpdate >= _variationRate) {
    _lastUpdate = ttime;
    _currentColor = !_currentColor;

    if (_currentColor) {
      _setRGB(_colB[0], _colB[1], _colB[2]);
    } else {
      _setRGB(_colA[0], _colA[1], _colA[2]);
    }
  }
}

void Alarm::_testingState() {
  unsigned long ttime = millis();
  if (ttime - _testStartTime > _timeoutDelay ) {
    _state = OFF;
    _turnOff();
    return;
  }

  digitalWrite(_buzzerPin, HIGH);

  if (ttime - _lastUpdate >= _variationRate) {
    _lastUpdate = ttime;
    _currentColor = !_currentColor;

    if (_currentColor) {
      _setRGB(_colA[0], _colA[1], _colA[2]);
    } else {
      _setRGB(_colB[0], _colB[1], _colB[2]);
    }
  }
}