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
  //Serial.println("Time:" + String(_index) + " " + String(time));
 
  float t = (time - _startTime) % (unsigned int)(4.0 * _delta);
  
  if(!isOn ||  (t >= (_delta * (float)_index + _injectionTime)))
  {
    if(isActivated)
    {
      deactivate();
    }
  }
  else if(isOn && (t >= _delta * (float)_index))
  {
    if(!isActivated)
    {
      activate();
    }
  }
}

void Injector:: activate()
{
  digitalWrite(_pin, HIGH);
  isActivated = true;
  Serial.println("Injector on: " + String(_index));
}

void Injector:: deactivate()
{
  digitalWrite(_pin, LOW);
  isActivated = false;
  Serial.println("Injector off: " + String(_index));
}

void Injector:: setOn()
{
  isOn = true;
}

void Injector:: setOff()
{
  isOn = false;
}

bool Injector:: getIsOn()
{
  return isOn;
}
