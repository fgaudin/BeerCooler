#ifndef Settings_h
#define Settings_h

#include "Arduino.h"

class Settings
{
  public:
    Settings(int minLimit, int maxLimit);
    int incrementHigh();
    int decrementHigh();
    int incrementLow();
    int decrementLow();
    int getLow();
    int getHigh();
    unsigned long getLastUpdate();
    void update();
    int compare(float value);
  private:
    unsigned long _lastUpdate;
    int _minLimit;
    int _maxLimit;
    int _lowLimit;
    int _highLimit;
    void _save();
    void _load();
};

#endif
