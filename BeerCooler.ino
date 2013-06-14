#include <EEPROM.h>
#include <PushButton.h>
#include <Led7Seg.h>
#include <OneWire.h>
#include "Settings.h"

#define MIN_TEMPERATURE 60
#define MAX_TEMPERATURE 79

OneWire tempSensor(3);
PushButton buttonHigh(4);
PushButton buttonLow(5);
Settings settings(MIN_TEMPERATURE, MAX_TEMPERATURE);
Led7Seg leds(11, 10, 8, 7, 6, 12, 13, 9);
int fanPin = 2;

// settings button
#define MODE_DISPLAY 0
#define MODE_SET_LOW 1
#define MODE_SET_HIGH 2

const int delayBeforeResetMode = 4000;

int mode = MODE_DISPLAY;

// temperature
const unsigned long delayBetweenTempCheck = 2000;
float temperature = 0;
boolean converting = false;
unsigned long lastTempCheck = 0;

void setup(void) {
  Serial.begin(9600);
  configureTemperaturePrecision();
  pinMode(fanPin, OUTPUT);
}

void loop(void) {
  checkButtons();
  checkTemperature();
  leds.refresh();
}

void checkButtons(){
  boolean highButtonState = buttonHigh.getState();
  boolean lowButtonState = buttonLow.getState();
  if (mode == MODE_DISPLAY){
    if (highButtonState == PushButton::PRESSED){
      settings.update();
      mode = MODE_SET_HIGH;
      leds.blink(true);
      leds.displayH();
      delay(500);
      displayTemperature(settings.getHigh());
    } else if (lowButtonState == PushButton::PRESSED){
      settings.update();
      mode = MODE_SET_LOW;
      leds.blink(true);
      leds.displayL();
      delay(500);
      displayTemperature(settings.getLow());
    }
  } else {
    if (highButtonState == PushButton::PRESSED){
      if (mode == MODE_SET_HIGH) {
        displayTemperature(settings.incrementHigh());
      } else if (mode == MODE_SET_LOW) {
        displayTemperature(settings.incrementLow());
      }
    } else if (lowButtonState == PushButton::PRESSED){
      if (mode == MODE_SET_HIGH) {
        displayTemperature(settings.decrementHigh());
      } else if (mode == MODE_SET_LOW) {
        displayTemperature(settings.decrementLow());
      }
    }

    int elapsedTime = millis() - settings.getLastUpdate();
    if (abs(elapsedTime) > delayBeforeResetMode) {
      mode = MODE_DISPLAY;
      leds.blink(false);
      leds.clear();
      delay(500);
    }
  }
}

// temperature
void displayTemperature(float temperature) {
  if(temperature < MIN_TEMPERATURE) {
    leds.dot(false);
    leds.displayMinus();
  } else if(MIN_TEMPERATURE <= temperature && temperature < MIN_TEMPERATURE + 10) {
    leds.dot(false);
    leds.display(temperature - MIN_TEMPERATURE);
  } else if(MIN_TEMPERATURE + 10 <= temperature && temperature < MAX_TEMPERATURE + 1) {
    leds.dot(true);
    leds.display(temperature - 10 - MIN_TEMPERATURE);
  } else {
    leds.dot(true);
    leds.displayNothing();
  }
}

void checkTemperature(){
  unsigned long now = millis();
  if (abs(now - lastTempCheck) > delayBetweenTempCheck) {
    if(readTemp()){
      Serial.println(temperature);
      if (mode == MODE_DISPLAY) {
        displayTemperature(temperature);
      }
      
      switch(settings.compare(temperature)) {
        case 1:
          digitalWrite(fanPin, HIGH);
          break;
        case -1:
          digitalWrite(fanPin, LOW);
          break;
      }
    }
  }
}

// temperature sensor
void configureTemperaturePrecision(){
  tempSensor.reset();
  tempSensor.skip();
  tempSensor.write(0x4E);
  tempSensor.write(0xD0);
  tempSensor.write(0x00);
  tempSensor.write(0b00011111);
}

void convertTemperature(){
  tempSensor.reset();
  tempSensor.skip();
  tempSensor.write(0x44);
}

void readScratchpad(){
  byte i;
  byte data[12];
  byte addr[8];
  float celsius, fahrenheit;
  
  tempSensor.reset();  
  tempSensor.skip();
  tempSensor.write(0xBE);         // Read Scratchpad

  for ( i = 0; i < 9; i++) {           // we need 9 bytes
    data[i] = tempSensor.read();
  }

  // Convert the data to actual temperature
  // because the result is a 16 bit signed integer, it should
  // be stored to an "int16_t" type, which is always 16 bits
  // even when compiled on a 32 bit processor.
  int16_t raw = (data[1] << 8) | data[0];
  byte cfg = (data[4] & 0x60);
  // at lower res, the low bits are undefined, so let's zero them
  if (cfg == 0x00) raw = raw & ~7;  // 9 bit resolution, 93.75 ms
  else if (cfg == 0x20) raw = raw & ~3; // 10 bit res, 187.5 ms
  else if (cfg == 0x40) raw = raw & ~1; // 11 bit res, 375 ms
  //// default is 12 bit resolution, 750 ms conversion time

  celsius = (float)raw / 16.0;
  fahrenheit = celsius * 1.8 + 32.0;
  temperature = fahrenheit;
}

boolean isTempSensorBusy(){
  return !tempSensor.read_bit();
}

boolean readTemp(){
  if(!isTempSensorBusy()){
    if(!converting){
      convertTemperature();
      converting = true;
    } else {
      readScratchpad();
      converting = false;
      lastTempCheck = millis();
      return true;
    }
  }
  return false;
}
// end temperature sensor
