#include <Arduino.h>
#include <Wire.h>
#include "MAX30100_PulseOximeter.h"
#include  <LiquidCrystal_I2C.h>
#include <Adafruit_MLX90614.h>
//#include "Temperature.cpp"

#define REPORTING_PERIOD_MS 1000
#define Buzzer 2
PulseOximeter pox;
LiquidCrystal_I2C LCD(0x3F,16,2);
Adafruit_MLX90614 GY906 = Adafruit_MLX90614();

uint32_t timeLastReport = 0;


uint8_t heart[8] = {0x0,0xa,0x1f,0x1f,0xe,0x4,0x0};

void onBeatDetected();
void MAX30100_PulseOximeter_Initializing();
void printData();
void GY906_Initializing();
void makeSound(byte code, byte times);


void onBeatDetected(){
    makeSound(2,1);
    LCD.setCursor(11,0);
    LCD.print("BEAT!");
}
void MAX30100_PulseOximeter_Initializing(){
  LCD.clear();
  if (pox.begin())
  { 
    LCD.setCursor(0,0);
    LCD.print("MAX30100:");
    LCD.setCursor(1,1);
    LCD.print("SUCCESS");
    makeSound(1,1);
  } else{
    LCD.setCursor(0,0);
    LCD.print("MAX30100:");
    LCD.setCursor(1,1);
    LCD.print("FAILED");
  }
  pox.setOnBeatDetectedCallback(onBeatDetected);
  delay(300);
}
void GY906_Initializing(){
  LCD.clear();
  if (GY906.begin()){
    LCD.setCursor(0,0);
    LCD.print("GY906:");
    LCD.setCursor(1,1);
    LCD.print("SUCCESS");
    makeSound(1,1);
  } else
  {
    LCD.setCursor(0,0);
    LCD.print("GY906:");
    LCD.setCursor(1,1);
    LCD.print("FAILED");
  }
  delay(500);
}
void printData(float heartRate, float SpO2){
    LCD.clear();
    LCD.setCursor(0,0);
    LCD.write(0);

    LCD.setCursor(0,1);
    LCD.print("SpO2:");

    LCD.setCursor(5,0);
    LCD.print(heartRate);
    LCD.setCursor(5,1);
    LCD.print(SpO2);
}

void setup() {
  
  LCD.init();
  LCD.backlight();
  LCD.setCursor(0,0);
  LCD.print("DANG KHOI TAO");
  delay(500);
  LCD.createChar(0,heart);
  
  pinMode(Buzzer, OUTPUT);
  
  
  MAX30100_PulseOximeter_Initializing();
  GY906_Initializing();

  LCD.clear();
  
  // put your setup code here, to run once:
}

void makeSound(byte code, byte times){
  byte i;
  switch (code)
  {
  case 1/* constant-expression */:
    /* code */
    for (i = 1; i <= times; i++){
      tone(Buzzer,220,125);
      delay(50);
    }
    break;
  case 2:
    for (i = 1; i <= times; i++){
      tone(Buzzer,2093,250);
      delay(50);
    }
    break;
  case 3:
    for (i = 1; i <= times; i++){
      tone(Buzzer,82,125);
      delay(50);
    }
    break;
  default:
    break;
  }
  /*tone(Buzzer,220,125);
  tone(Buzzer,2093,250);
  tone(Buzzer,82,125);*/
}
void loop() {
  // put your main code here, to run repeatedly:
  //pox.resume();
  pox.update();
  if (millis() - timeLastReport > REPORTING_PERIOD_MS){
    float heartRate = pox.getHeartRate();
    float SpO2 = pox.getSpO2();
    printData(heartRate,SpO2);
    timeLastReport = millis();
    LCD.setCursor(11,1);
    LCD.print(GY906.readObjectTempC());
  }
}