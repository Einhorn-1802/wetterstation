#include <ESP8266WiFi.h>        // Include the Wi-Fi library
//#include <WiFiUdp.h>
#include "wlanData.h"
//#include <OneWire.h>
//#include <DallasTemperature.h>
#include <BlynkSimpleEsp8266.h>
#include <time.h>
#include <Wire.h>
//#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>


extern "C" {
#include "user_interface.h"
}

//#define BME_SCK 13
//#define BME_MISO 12
//#define BME_MOSI 11
//#define BME_CS 10

#define SEALEVELPRESSURE_HPA (1013.25)
Adafruit_BME280 bme; // I2C

#define RTCMEMORYSTART 75
#define MY_NTP_SERVER "de.pool.ntp.org"           
#define MY_TZ "CET-1CEST,M3.5.0/02,M10.5.0/03"   



#define BLYNK_PRINT Serial

//#define ONE_WIRE_PIN 12
 
//OneWire oneWire(ONE_WIRE_PIN);
//DallasTemperature sensors(&oneWire);

char auth[] = BLYNK_AUTH_TOKEN;


time_t now;
tm tm;

int keepAlive;

typedef struct {
  int rainCount;
  int year;
  int y_day;
} rtcStore;

rtcStore rtcMem;

int wakeupReason;

void readFromRTCMemory() {
  system_rtc_mem_read(RTCMEMORYSTART, &rtcMem, sizeof(rtcMem));
  yield();
}
void writeToRTCMemory() {
  system_rtc_mem_write(RTCMEMORYSTART, &rtcMem, sizeof(rtcMem));
  yield();
}

void setup() {
  // put your setup code here, to run once:
    Serial.begin(115200);         // Start the Serial communication to send messages to the computer
    wakeupReason = digitalRead(13);
    readFromRTCMemory();
    Serial.println("");
    Serial.print("Wakeup Reason:");
    Serial.println(wakeupReason);
    Serial.print("Rain Count: ");
    Serial.println(rtcMem.rainCount);
    Serial.println(rtcMem.year);
    Serial.println(rtcMem.y_day);
    if (wakeupReason == 0) {
      rtcMem.rainCount++;
      writeToRTCMemory();
    } 
    unsigned status;
    status = bme.begin(0x76);
    if (!status) {
        Serial.println("Could not find a valid BME280 sensor, check wiring, address, sensor ID!");
        Serial.print("SensorID was: 0x"); Serial.println(bme.sensorID(),16);
        Serial.print("        ID of 0xFF probably means a bad address, a BMP 180 or BMP 085\n");
        Serial.print("   ID of 0x56-0x58 represents a BMP 280,\n");
        Serial.print("        ID of 0x60 represents a BME 280.\n");
        Serial.print("        ID of 0x61 represents a BME 680.\n");
        while (1) delay(10);
    }
   
    Blynk.begin(auth, ssid, password);
    pinMode(D4, OUTPUT);
    time(&now);                       // read the current time
    localtime_r(&now, &tm);           // update the structure tm with the current time
 
    int current_year = tm.tm_year + 1900;
    int year_day = tm.tm_yday;
    int day_hour = tm.tm_hour;
    Serial.print("Current Year: ");
    Serial.println(current_year);
    Serial.print("Day of the Year: ");
    Serial.println(year_day);

    if ((current_year == rtcMem.year && year_day == rtcMem.y_day) || now < 1) {
    }
    else {
      if (wakeupReason == 0) {
        rtcMem.rainCount = 1;
      }
      else {
        rtcMem.rainCount = 0;
      }
      rtcMem.year = current_year;
      rtcMem.y_day = year_day;
      writeToRTCMemory();
    } 
//    sensors.begin();
//    sensors.requestTemperatures();
//    Serial.println(sensors.getTempCByIndex(0));
    Blynk.virtualWrite(V0, bme.readTemperature());
    Blynk.virtualWrite(V1, (double)rtcMem.rainCount * 0.25);
    Blynk.virtualWrite(V3, bme.readPressure() / 100.0F);
    Blynk.virtualWrite(V4, bme.readHumidity());
}

void loop() {   
  Blynk.syncVirtual(V2);
  Blynk.run();
  Blynk.syncVirtual(V2);

   if (keepAlive != 1) {
      ESP.deepSleep(1e6*300);
   }
}   

BLYNK_WRITE(V2)
{
  keepAlive= param.asInt();
}
