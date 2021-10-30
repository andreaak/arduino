#include "OneButton.h"
#include "Adafruit_GFX.h"
#include "Adafruit_SSD1306.h"
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
Adafruit_SSD1306 lcd(128, 64, &Wire, 4); // указываем размер экрана в пикселях

bool isOn = true;
int rpm = 100;
int rpmDelta = 50;
int rpmDeltaMin = 10;

int injectionTime = 18;
int injectionTimeDelta = 1;
int currentMode = 0;
int currentInjMode = 0;

int RPM_MODE = 1;
int TIME_MODE = 2;
int INJ_MODE = 3;

int OFF_MODE = 0;
int ON_MODE = 1;

unsigned long startTime = millis();

Injector injector1(INJECTOR1, 0, rpm, injectionTime, startTime); 
Injector injector2(INJECTOR2, 1, rpm, injectionTime, startTime); 
Injector injector3(INJECTOR3, 2, rpm, injectionTime, startTime); 
Injector injector4(INJECTOR4, 3, rpm, injectionTime, startTime); 

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
}

void handleModeClick()
{
  currentMode = ++currentMode % 4;
  isDataDisplayed = false;
}

void handleUpClick()
{
  if(currentMode == 0)
  {
    return;
  }
  if(currentMode == RPM_MODE)
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
  else if(currentMode == TIME_MODE)
  {
    injectionTime += injectionTimeDelta;
    setInjectionTime();
  }
  else if(currentMode == TIME_MODE)
  {
    injectionTime += injectionTimeDelta;
    setInjectionTime();
  }
  else if(currentMode == INJ_MODE)
  {
    currentInjMode = ++currentInjMode % 6;
  }  
  isDataDisplayed = false;
}

void handleDownClick()
{
  if(currentMode == 0)
  {
    return;
  }
  if(currentMode == RPM_MODE)
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
  else if(currentMode == TIME_MODE)
  {
    if(injectionTime > 4)
    {
      injectionTime -= injectionTimeDelta;
      setInjectionTime();
    }
  }
  else if(currentMode == INJ_MODE)
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
  if(currentInjMode == 0)
  {
    injector1.setOff();
    injector2.setOff();
    injector3.setOff();
    injector4.setOff();
  }
  else if(currentInjMode == 1)
  {
    injector1.setOn();
    injector2.setOn();
    injector3.setOn();
    injector4.setOn();
  }
  else if(currentInjMode == 2)
  {
    injector1.setOn();
    injector2.setOff();
    injector3.setOff();
    injector4.setOff();    
  }
  else if(currentInjMode == 3)
  {
    injector1.setOff();
    injector2.setOn();
    injector3.setOff();
    injector4.setOff();    
  }
  else if(currentInjMode == 4)
  {
    injector1.setOff();
    injector2.setOff();
    injector3.setOn();
    injector4.setOff();    
  }
  else if(currentInjMode == 5)
  {
    injector1.setOff();
    injector2.setOff();
    injector3.setOff();
    injector4.setOn();   
  }        
}

void displayData()
{
  if(isDataDisplayed)
  {
    return;
  }

  lcd.clearDisplay();  

  if(currentMode == RPM_MODE)
  {
    blinkRpm();
  } 
  else if(currentMode == TIME_MODE)
  {
    blinkInjectionTime();
  }
  else if(currentMode == INJ_MODE)
  {
    blinkInjectorMode();
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
  
  if(currentInjMode == OFF_MODE)
  {
    lcd.println("Off");
  }
  else if(currentInjMode == ON_MODE)
  {
    lcd.println("On");
  }
  else if(currentInjMode == 2)
  {
    lcd.println("1");
  }
  else if(currentInjMode == 3)
  {
    lcd.println("2");
  }
  else if(currentInjMode == 4)
  {
    lcd.println("3");
  }
  else if(currentInjMode == 5)
  {
    lcd.println("4");
  }
  else
  {
    lcd.println("---");
  }
  lcd.display();
}
