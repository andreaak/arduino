#include "OneButton.h"
#include "Adafruit_GFX.h"
#include "Adafruit_SSD1306.h"
#include "Injector.h"


#define MODE_BUTTON 10
#define UP_BUTTON 11
#define DOWN_BUTTON 12

#define INJECTOR1 14
#define INJECTOR2 15
#define INJECTOR3 16
#define INJECTOR4 17

#define ALL_MODE 0
#define I1_MODE 1
#define I2_MODE 2
#define I3_MODE 3
#define I4_MODE 4

#define NO_SELECTION_MODE 0
#define RPM_MODE 1
#define TIME_MODE 2
#define INJ_MODE 3

#define MIN_INJ_TIME 3

OneButton buttonMode(MODE_BUTTON, true, true);
OneButton buttonUp(UP_BUTTON, true, true);
OneButton buttonDown(DOWN_BUTTON, true, true);
Adafruit_SSD1306 lcd(128, 64, &Wire, -1); // указываем размер экрана в пикселях

unsigned long startTime = millis();
int rpm = 150;
int injectionTime = 18;

Injector injector1(INJECTOR1, 0, rpm, injectionTime, startTime); 
Injector injector2(INJECTOR2, 1, rpm, injectionTime, startTime); 
Injector injector3(INJECTOR3, 2, rpm, injectionTime, startTime); 
Injector injector4(INJECTOR4, 3, rpm, injectionTime, startTime); 

int rpmDelta = 50;
int rpmDeltaMin = 10;

int injectionTimeDelta = 1;

int currentSelectMode = NO_SELECTION_MODE;
int currentInjMode = ALL_MODE;

bool isOn = false;

bool isDataDisplayed = false;

void setup() {
  Serial.begin(115200);
  //Serial.println("One Button Example with polling.");

  lcd.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  // Print a message to the LCD.
  lcd.clearDisplay();
  lcd.setTextSize(1, 2);
  lcd.setTextColor(SSD1306_WHITE); // указываем цвет надписи
  lcd.display();
  
  buttonMode.attachClick(handleModeClick);
  buttonUp.attachClick(handleUpClick);
  buttonDown.attachClick(handleDownClick);
}

void loop() {

  buttonMode.tick();
  if(currentSelectMode != 0)
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
}

void handleModeClick()
{
  currentSelectMode = ++currentSelectMode % 4;
  isDataDisplayed = false;
}

void handleUpClick()
{
  if(currentSelectMode == NO_SELECTION_MODE)
  {
    return;
  }
  if(currentSelectMode == RPM_MODE)
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
  else if(currentSelectMode == TIME_MODE)
  {
    injectionTime += injectionTimeDelta;
    setInjectionTime();
  }
  else if(currentSelectMode == INJ_MODE)
  {
    currentInjMode = ++currentInjMode % 5;
  }  
  isDataDisplayed = false;
}

void handleDownClick()
{
  if(currentSelectMode == NO_SELECTION_MODE)
  {
    return;
  }
  if(currentSelectMode == RPM_MODE)
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
  else if(currentSelectMode == TIME_MODE)
  {
    if(injectionTime > MIN_INJ_TIME)
    {
      injectionTime -= injectionTimeDelta;
      setInjectionTime();
    }
  }
  else if(currentSelectMode == INJ_MODE)
  {
    setInjectionMode();
  }
  isDataDisplayed = false;
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

void setInjectionMode()
{ 
  switch(currentInjMode)
  {
    case ALL_MODE:
      if(isOn)
      {
        turnOff();
      }
      else
      {
        turnOn();
      }
      break;
    case I1_MODE:
      switchInjector1();
      break;  
    case I2_MODE:
      switchInjector2();
      break;  
    case I3_MODE:
      switchInjector3();
      break;
    case I4_MODE:
      switchInjector4();
      break;
  }
}

void turnOn()
{
    isOn = true;
    injector1.setOn();
    injector2.setOn();
    injector3.setOn();
    injector4.setOn();
}

void turnOff()
{
    isOn = false;
    injector1.setOff();
    injector2.setOff();
    injector3.setOff();
    injector4.setOff();
}

void switchInjector1()
{
    switchInjector(&injector1);
//    injector2.setOff();
//    injector3.setOff();
//    injector4.setOff(); 
}

void switchInjector2()
{
//    injector1.setOff();
    switchInjector(&injector2);
//    injector3.setOff();
//    injector4.setOff(); 
}

void switchInjector3()
{
//    injector1.setOff();
//    injector2.setOff();
    switchInjector(&injector3);
//    injector4.setOff(); 
}

void switchInjector4()
{
//    injector1.setOff();
//    injector2.setOff();
//    injector3.setOff();
    switchInjector(&injector4); 
}

void switchInjector(Injector* injector)
{
    Serial.println(injector->getIsOn());
    if(injector->getIsOn())
    {
      injector->setOff();
    }
    else
    {
      injector->setOn();
    }
}

void displayData()
{
  if(isDataDisplayed)
  {
    return;
  }

  lcd.clearDisplay();  

  switch(currentSelectMode)
  {
    case RPM_MODE:
      blinkRpm();
      break;
    case TIME_MODE:
      blinkInjectionTime();
      break;
    case INJ_MODE:
      blinkInjectorMode();
      break;  
  }
  lcd.setCursor(10, 0);
  lcd.print("RPM:  ");
  lcd.println(rpm);
  
  lcd.setCursor(10, 16);
  lcd.print("TIME: ");
  lcd.println(injectionTime);

  setInjectionModeDescription();

  lcd.display();
  
  isDataDisplayed = true;
}

void blinkRpm()
{
  lcd.setCursor(0,0);
  lcd.print("|");
  lcd.display();
}

void blinkInjectionTime()
{
  lcd.setCursor(0,16);
  lcd.print("|");
  lcd.display();
}

void blinkInjectorMode()
{
  lcd.setCursor(0,32);
  lcd.print("|");
  lcd.display();
}

void setInjectionModeDescription()
{
  lcd.setCursor(10, 32);
  lcd.print("INJ:  ");
  
  switch(currentInjMode)
  {
    case ALL_MODE:
      if(isOn)
      {
        lcd.println("All On");
      }
      else
      {
        lcd.println("All Off");
      }
      break;
    case I1_MODE:
      lcd.print("1");
      lcd.println(injector1.getIsOn() ? " On": " Off");
      break;  
    case I2_MODE:
      lcd.print("2");
      lcd.println(injector2.getIsOn() ? " On": " Off");
      break;  
    case I3_MODE:
      lcd.print("3");
      lcd.println(injector3.getIsOn() ? " On": " Off");
      break;
    case I4_MODE:
      lcd.print("4");
      lcd.println(injector4.getIsOn() ? " On": " Off");
      break;
  }
  lcd.display();
}
