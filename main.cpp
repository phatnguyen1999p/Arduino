#include <Arduino.h>
#include <Wire.h>
#include <MAX30100_PulseOximeter.h>
#include <LiquidCrystal_I2C.h>
#include <Adafruit_MLX90614.h>
#include <RtcDS1307.h>

#define REPORTING_PERIOD_MINUTE 1
#define Buzzer 2
#define countof(a) (sizeof(a)/ sizeof(a[0]))

PulseOximeter pox;
LiquidCrystal_I2C LCD(0x3F,16,2);
Adafruit_MLX90614 GY906 = Adafruit_MLX90614(0x5A);
RtcDS1307<TwoWire> RTC(Wire);

uint8_t timeLastReport = 0;

char TIME[9];
char datestring[11];
int lastSecond,lastDay;
uint8_t heart[8] = {0x0,0xa,0x1f,0x1f,0xe,0x4,0x0};

void onBeatDetected();
void MAX30100_PulseOximeter_Initializing();
void printData(float, float, float,float);
void GY906_Initializing();
void makeSound(byte code, byte times);
void printDateTime(const RtcDateTime& dt);
//void DELAY(int TIME);

void onBeatDetected(){
    makeSound(2,1);
    LCD.setCursor(10,0);
    LCD.write(0);
}
void MAX30100_PulseOximeter_Initializing(){
  LCD.clear();
  if (pox.begin()==true)
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
    makeSound(1,2);
  }
  pox.setOnBeatDetectedCallback(onBeatDetected);
}
void GY906_Initializing(){
  LCD.clear();
  if (GY906.begin()==true){
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
    makeSound(1,2);
  }
  //delay(1000);
}
void RTC_Initializing(){
  RTC.Begin();
  RtcDateTime complied = RtcDateTime(__DATE__,__TIME__);
  printDateTime(complied);

  if (!RTC.IsDateTimeValid()){
    if (RTC.LastError() != 0){
      Serial.print("RTC communications ERROR = ");
      Serial.println(RTC.LastError());
    }else
    {
      Serial.println("RTC lost confidence in the time");
      RTC.SetDateTime(complied);
    }
  }

  RtcDateTime now = RTC.GetDateTime();
  if (!RTC.GetIsRunning()){
    Serial.println("RTC was not actively running");
    RTC.SetIsRunning(true);
  }
  if (now < complied){
    RTC.SetDateTime(complied);
  }
  else if(now > complied){
  }
  else if (now == complied){
  }
  now = RTC.GetDateTime();
  lastDay = now.Day();
  lastSecond = now.Second();
  RTC.SetSquareWavePin(DS1307SquareWaveOut_Low);

  snprintf_P(datestring,
  countof(datestring), 
  PSTR("%02u/%02u/%04u"), 
  now.Day(),
  now.Month(),
  now.Year());
}
void printData(float heartRate, float SpO2, float ATemp, float OTemp){
    LCD.clear();

    LCD.setCursor(0,0);
    LCD.print("BPM:");
    LCD.setCursor(0,1);
    LCD.print("SpO2:");

    LCD.setCursor(5,0);
    LCD.print(heartRate);
    LCD.setCursor(5,1);
    LCD.print(SpO2);
    LCD.setCursor(10,1);
    LCD.print("%");

    LCD.setCursor(11,0);
    LCD.print(ATemp);
    LCD.setCursor(11,1);
    LCD.print(OTemp);
}
void printDateTime(const RtcDateTime& dt){

  LCD.clear();
  if (dt.Day() != lastDay){
    snprintf_P(datestring,
    countof(datestring), 
    PSTR("%02u/%02u/%04u"), 
    dt.Day(),
    dt.Month(),
    dt.Year());
    lastDay = dt.Day();
  }
  snprintf_P(TIME,
   countof(TIME), 
   PSTR("%02u:%02u:%02u"), 
   dt.Hour(), 
   dt.Minute(), 
   dt.Second());

  LCD.setCursor(0,0);
  LCD.print(datestring);
  LCD.setCursor(0,1);
  LCD.print(TIME);
}
void setup() {
  Wire.setClock(I2C_BUS_SPEED);
  /*digitalWrite( SDA, HIGH);
  digitalWrite( SCL, HIGH);
  digitalWrite( SDA, LOW);
  digitalWrite( SCL, LOW);*/
  
  LCD.init();
  LCD.backlight();
  LCD.setCursor(0,0);
  LCD.print("DANG KHOI TAO");
  delay(500);
  LCD.createChar(0,heart);
  
  pinMode(Buzzer, OUTPUT);
  makeSound(2, 3);

  RTC_Initializing();
  MAX30100_PulseOximeter_Initializing();
  GY906_Initializing();
  
  Serial.begin(9600);
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
      tone(Buzzer,220,250);
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
  pox.update();
  RtcDateTime now = RTC.GetDateTime();
  if (now.Minute() - timeLastReport >= REPORTING_PERIOD_MINUTE){
    
    float heartRate = pox.getHeartRate();
    float SpO2 = pox.getSpO2();
    double OTemp = GY906.readObjectTempC();
    double ATemp = GY906.readAmbientTempC();
  
    printData(heartRate,SpO2,ATemp,OTemp);
    
    timeLastReport = now.Minute();
    if (timeLastReport == 59){
      timeLastReport = -1;
    }
    pox.shutdown();
    makeSound(2,3);    
    delay(3000);
    pox.resume();
  }
  else if (now.Second() != lastSecond){
    printDateTime(now);
    lastSecond = now.Second();
  }
}