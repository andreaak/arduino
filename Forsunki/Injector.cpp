#include "Injector.h"
#include "Arduino.h"

Injector::Injector(const int pin, const int index, const int rpm, const float injectionTime, const unsigned long startTime)
{

  _pin = pin;
  _index = index;
  _rpm = rpm;
  _injectionTime = injectionTime;
  _startTime = startTime;
  pinMode(pin, OUTPUT);
  setDelta();
}

void Injector::setRpm(const int rpm)
{
  _rpm = rpm;
  setDelta();
}

void Injector::setInjectionTime(const float injectionTime)
{
  _injectionTime = injectionTime;
}

void Injector::setDelta()
{
  _delta = 60.0 / 2.0 / (float)_rpm * 1000.0;//ms
}

void Injector:: process(unsigned long time)
{
  Serial.println("Time:" + String(_index) + " " + String(time));
 
  float t = (time - _startTime) % (unsigned int)(4.0 * _delta);
  
  if(t >= (_delta * (float)_index + _injectionTime))
  {
    if(isOn)
    {
      digitalWrite(_pin, LOW);
      isOn = false;
      Serial.println("Injector off: " + String(_index));
    }
  }
  else if(t >= _delta * (float)_index)
  {
    if(!isOn)
    {
      digitalWrite(_pin, HIGH);
      isOn = true;
      Serial.println("Injector on: " + String(_index));
    }
  }
}
