#include "Arduino.h"
#include "Settings.h"
#include <EEPROM.h>

Settings::Settings(int minLimit, int maxLimit){
  _lastUpdate = 0;
  _minLimit = minLimit;
  _maxLimit = maxLimit;
  _load();
}

int Settings::incrementHigh(){
  update();
  _highLimit = min(_highLimit + 1, _maxLimit);
  _save();
  return _highLimit;
}

int Settings::decrementHigh(){
  update();
  _highLimit = max(_highLimit - 1, _minLimit);
  _save();
  return _highLimit;
}

int Settings::incrementLow(){
  update();
  _lowLimit = min(_lowLimit + 1, _maxLimit);
  _save();
  return _lowLimit;
}

int Settings::decrementLow(){
  update();
  _lowLimit = max(_lowLimit - 1, _minLimit);
  _save();
  return _lowLimit;
}

int Settings::getLow(){
  return _lowLimit;
}

int Settings::getHigh(){
  return _highLimit;
}

unsigned long Settings::getLastUpdate(){
  return _lastUpdate; 
}

void Settings::update(){
  _lastUpdate = millis();
}

int Settings::compare(float value){
  if (_lowLimit < _highLimit){
    if (value < _lowLimit){
      return -1;
    } else if (value > _highLimit) {
      return 1;
    }
  } else {
    if (value > _lowLimit){
      return -1;
    } else if (value < _highLimit) {
      return 1;
    }
  }
  return 0;
}

void Settings::_load(){
  _lowLimit = int(EEPROM.read(0));
  if (_lowLimit == 255) {
    _lowLimit = _minLimit;
  }
  _highLimit = int(EEPROM.read(1));
  if (_highLimit == 255) {
    _highLimit = _maxLimit;
  }
  _save();
}

void Settings::_save(){
  EEPROM.write(0, byte(_lowLimit));
  EEPROM.write(1, byte(_highLimit));
}
