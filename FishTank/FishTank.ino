/* Comment this out to disable prints and save space */
#define BLYNK_PRINT Serial
#define lightPin D0 //กำหนดขา D0 เปิดปิด LED
#define SDAPin D2 // for lightMetter
#define SCLPin D1 // for lightMetter
#define ServoPin 14 //กำหนดขาservoเป็นD5

#define ONE_WIRE_BUS 2 //กำหนดขา D4 เชื่อมต่อ Sensor วัดอุณหภูมิเพื่อรับข้อมูล 
#define pumpPin D3 //ขา D3 เชื่อมปั้มน้ำ

/* Fill in information from Blynk Device Info here */
#define BLYNK_TEMPLATE_ID "TMPL62SgXAcI9"
#define BLYNK_TEMPLATE_NAME "IOTProject"
#define BLYNK_AUTH_TOKEN "xo8NenYP7jamB342CcskMzrXWoXEKH0f"

#include <Servo.h>
#include <OneWire.h> //Library โปรโตคอล OneWire
#include <DallasTemperature.h> //Library สำหรับจัดการข้อมูลที่ได้จาก DS18B20
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <Wire.h> //Library สำหรับ I2C
#include <BH1750.h>

BH1750 LightMeter(0x23); //สร้าง Obj สำหรับใช้ BH1750
Servo myservo; //สร้าง Obj สำหรับใช้ Servo

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sTemp(&oneWire);

// Your WiFi credentials.
// Set password to "" for open networks.

const char ssid[] = "TEMPURA-0143";
const char pass[] = "12351235";

unsigned long startTime = 0;  // ตัวแปรสำหรับจับเวลา (unsigned เก็บค่าลบไม่ได้ long จำนวนเต็ม > int)
bool timerStarted = false;   // ตัวแปรสถานะจับเวลา

bool auto_light_switch = false; //สำหรับเช็คว่าเปิด autoLight ไว้ไหม
bool LEDstate = false; //สำหรับเช็คว่าไฟเปิดอยู่หรือไม่

bool autoPump = false;
bool PUMPstate = false; //สำหรับเช็คว่าปั้มน้ำเปิดอยู่หรือไม่

int autoFeed; //สำหรับ Set จำนวนรวบให้อาหารใน 1 วัน
unsigned long lastFeedTime = 0; //เก็บเวลาที่ให้อาหารล่าสุด
bool feedTimerStarted = false;//สถานะการจับเวลา

void setup()
{
  myservo.attach(ServoPin); //กำหนดขา D5 ควบคุม servo
  Serial.begin(9600);

  pinMode(pumpPin, OUTPUT);
  digitalWrite(pumpPin, LOW);
  
  pinMode(lightPin, OUTPUT);
  digitalWrite(lightPin, LOW);
  
  Wire.begin(D2, D1); // (SDA, SCL)
  LightMeter.begin();

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  myservo.write(60);
}

void auto_light(bool auto_on){ //อ่านค่าแสงจาก sensor แล้วเปิด/ปิดไฟอัตโนมิติ
  double  lux = LightMeter.readLightLevel(); 

  if (!LEDstate && auto_on){
    if(lux <= 0 && !timerStarted){
      Blynk.virtualWrite(V1, 1); //เปิดไฟ
      digitalWrite(lightPin, 1);
      startTime = millis(); //เก็บค่าเวลาตอนนี้ไว้ใน startTime
      timerStarted = true;  //สถานะเรึ่มจับเวลา
      
    }
     
  }

  //ถ้าเวลาตอนนี้ลบเวลาที่เก็บไว้มากกว่าหรือ = 1.5 ชั่วโมง 
  if (timerStarted && millis() - startTime >= 5000) { //1.5 * 60 * 60 * 1000
    Blynk.virtualWrite(V1, 0); // ปิดไฟ
    digitalWrite(lightPin, 0);
    timerStarted = false;     // รีเซ็ตสถานะจับเวลา
  }
  
  Serial.print("LUX: ");
  Serial.println(lux);
}

void tem(){
  sTemp.requestTemperatures(); //สั่งให้เซ็นเซอร์เริ่มวัดอุณหภูมิ
  double temp = sTemp.getTempCByIndex(0); // อ่านอุณหภูมิจากเซ็นเซอร์ตัวแรก (index 0)
  
  Serial.print("Temperature is: ");
  Serial.print(temp); // แสดงค่า อูณหภูมิ 
  Serial.println(" *C");

  if(autoPump){
    if(!PUMPstate){
      if(temp >= 25)
        digitalWrite(pumpPin, 1);
      else digitalWrite(pumpPin, 0);
    }
  }else {
    if(PUMPstate)
      digitalWrite(pumpPin, 1);
    else digitalWrite(pumpPin, 0);
  }
  
  Blynk.virtualWrite(V3, temp); 
}

void feeding(){
  myservo.write(110);
  delay(1000);
  myservo.write(60);
}



void auto_feeding(){
  //ให้อาหารตามเวลาที่กำหนด
  if(feedTimerStarted && (millis() - lastFeedTime >= 24 * 60 * 60 *1000 / autoFeed)){ //17280 รอบต่อวัน = ทุก ๆ 5 วิ
    feeding();
    lastFeedTime = millis();
      
  }

}

BLYNK_WRITE(V0){    //เปิดปิดปั่มปั้ม
  int value = param.asInt();
  if (value >= 1) {
    digitalWrite(pumpPin, 1);
    PUMPstate = true;
  }
  else {
    digitalWrite(pumpPin, 0);
    PUMPstate = false;
  }
}

BLYNK_WRITE(V1){    //เปิดปิดไฟ 
  int value = param.asInt();
  if (value >= 1){
    digitalWrite(lightPin, 1);
    LEDstate = true;
  }
  else {
    digitalWrite(lightPin, 0);
    Blynk.virtualWrite(V5, 0);
    LEDstate = false;  
  }
  
}

BLYNK_WRITE(V2){    //Servo (ให้อาหารปลา)
  if (param.asInt() >= 1)
    feeding();
  lastFeedTime = millis();
}

BLYNK_WRITE(V4){    //Servo (ให้อาหารปลาอัตโนมัติ)
  autoFeed = param.asInt();
  feedTimerStarted = autoFeed > 0;
  lastFeedTime = millis();
  
}

BLYNK_WRITE(V5){    //light (เปิดไฟอัตโนมัติ)
  auto_light_switch = param.asInt();
}

BLYNK_WRITE(V6){    //เปิดปิดปั้มตามอุณภูมิ
  autoPump = param.asInt();

}

void loop()
{
  Blynk.run();
  auto_light(auto_light_switch);
  auto_feeding();
  tem();
  delay(2000);

}
