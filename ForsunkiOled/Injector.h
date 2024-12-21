#ifndef Injector_h
#define Injector_h


class Injector
{
public:

  Injector(const int pin, const int index, const int rpm, const float injectionTime, const unsigned long startTime);

  void setRpm(const int rpm);

  void setInjectionTime(const float injectionTime);

  void setOn();
  
  void setOff();

  bool getIsOn();

  void process(unsigned long time);


private:

  void setDelta();

  void activate();
  void deactivate();
  
  int _pin;
  int _index;
  int _rpm;
  float _injectionTime;
  unsigned long _startTime;
  float _delta;
  bool isOn;
  bool isActivated;	
};

#endif
