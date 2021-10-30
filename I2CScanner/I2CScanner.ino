#include <Wire.h>

void setup()
{
  Wire.begin();

  Serial.begin(9600);
  while (!Serial);  // Leonardo: wait for serial monitor
  Serial.println("\nI2C Сканер");
}


void loop()
{
  byte error, address;
  int nDevices;

  Serial.println("Сканирование...");

  nDevices = 0;
  for(address = 1; address < 127; address++ ) 
  {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();

    if (error == 0)
    {
      Serial.print("I2C устройство найдено по адресу 0x");
      if (address<16) 
        Serial.print("0");
      Serial.println(address,HEX);

      nDevices++;
    }
    else if (error==4) 
    {
      Serial.print("Неизвестная ошибка по адресу 0x");
      if (address<16) 
        Serial.print("0");
      Serial.println(address,HEX);
    }    
  }
  if (nDevices == 0)
    Serial.println("I2C устройство НЕ найдено\n");
  else
    Serial.println("Выполнено\n");

  delay(5000);
}
