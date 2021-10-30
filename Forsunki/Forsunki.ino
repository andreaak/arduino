#include "OneButton.h"
#include <LiquidCrystal_I2C.h>
#include "Injector.h"

#define INJECTOR1 5
#define INJECTOR2 6
#define INJECTOR3 7
#define INJECTOR4 8

#define MODE_BUTTON 4
#define UP_BUTTON 3
#define DOWN_BUTTON 2

OneButton buttonMode(MODE_BUTTON, true, true);
OneButton buttonUp(UP_BUTTON, true, true);
OneButton buttonDown(DOWN_BUTTON, true, true);
LiquidCrystal_I2C lcd(0x3F,16,2);

bool isOn = true;
int rpm = 100;
int rpmDelta = 50;
int rpmDeltaMin = 10;

float injectionTime = 18.0;
float injectionTimeDelta = 1;
int currentMode = 0;

unsigned long startTime = millis();

Injector injector1(INJECTOR1, 0, rpm, injectionTime, startTime); 
Injector injector2(INJECTOR2, 1, rpm, injectionTime, startTime); 
Injector injector3(INJECTOR3, 2, rpm, injectionTime, startTime); 
Injector injector4(INJECTOR4, 3, rpm, injectionTime, startTime); 

bool blinkRpmDisplayed = false;
bool blinkInjTimeDisplayed = false;
bool noBlinkDisplayed = false;
bool isDataDisplayed = false;

void setup() {
  Serial.begin(115200);
  Serial.println("One Button Example with polling.");

  lcd.init();
  // Print a message to the LCD.
  lcd.backlight();
  
  buttonMode.attachClick(handleModeClick);
  buttonUp.attachClick(handleUpClick);
  buttonDown.attachClick(handleDownClick);
}

void loop() {

  buttonMode.tick();
  if(currentMode != 0)
  {
    buttonUp.tick();
    buttonDown.tick(); 
  }

  unsigned long time = millis();

  injector1.process(time);
  injector2.process(time);
  injector3.process(time);
  injector4.process(time);

  displayData();
  displayBlink(); 
}

void handleModeClick()
{
  currentMode = ++currentMode % 3;
  refreshModeBlink();
}

void handleUpClick()
{
  if(currentMode == 0)
  {
    return;
  }
  if(currentMode == 1)
  {
    if((rpm + rpmDeltaMin) <= rpmDelta)
    {
      rpm += rpmDeltaMin;
    }
    else
    {
      rpm += rpmDelta;
    }
    
    setRpm();
  } 
  else if(currentMode == 2)
  {
    injectionTime += injectionTimeDelta;
    setInjectionTime();
  }
  isDataDisplayed = false;
  refreshModeBlink();
}

void handleDownClick()
{
  if(currentMode == 0)
  {
    return;
  }
  if(currentMode == 1)
  {
    if(rpm > rpmDelta)
    {
      rpm  -= rpmDelta;
      setRpm();
    }
    else if(rpm > rpmDeltaMin)
    {
      rpm  -= rpmDeltaMin;
      setRpm();
    }
  } 
  else if(currentMode == 2)
  {
    if(injectionTime > 4)
    {
      injectionTime -= injectionTimeDelta;
      setInjectionTime();
    }
  }
  isDataDisplayed = false;
  refreshModeBlink();
}

void setRpm()
{ 
  injector1.setRpm(rpm);
  injector2.setRpm(rpm);
  injector3.setRpm(rpm);
  injector4.setRpm(rpm);
}

void setInjectionTime()
{ 
  injector1.setInjectionTime(injectionTime);
  injector2.setInjectionTime(injectionTime);
  injector3.setInjectionTime(injectionTime);
  injector4.setInjectionTime(injectionTime);
}

void refreshModeBlink()
{ 
  noBlinkDisplayed = blinkRpmDisplayed = blinkInjTimeDisplayed = false;
}

void displayData()
{
  if(isDataDisplayed)
  {
    return;
  }

  lcd.clear();

  lcd.setCursor(1,0);
  lcd.print("RPM:  ");
  lcd.print(rpm);
  
  lcd.setCursor(1,1);
  lcd.print("TIME: ");
  lcd.print(injectionTime);
  isDataDisplayed = true;
}

void displayBlink()
{
  if(currentMode == 0 && !noBlinkDisplayed)
  {
    noBlinkDisplayed = true; 
    lcd.blink_off();
  } 
  else if(currentMode == 1 && !blinkRpmDisplayed)
  {
    blinkRpmDisplayed = true;
    blinkRpm();
  } 
  else if(currentMode == 2 && !blinkInjTimeDisplayed)
  {
    blinkInjTimeDisplayed = true;
    blinkInjectionTime();
  }
}

void blinkRpm()
{
  lcd.setCursor(0,0);
  lcd.blink_on();
}

void blinkInjectionTime()
{
  lcd.setCursor(0,1);
  lcd.blink_on();
}
