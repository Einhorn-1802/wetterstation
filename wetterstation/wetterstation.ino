#include <ESP8266WiFi.h>        // Include the Wi-Fi library
//#include <WiFiUdp.h>
#include "wlanData.h"
#include <OneWire.h>
#include <DallasTemperature.h>
#include <BlynkSimpleEsp8266.h>
#include <time.h>

extern "C" {
#include "user_interface.h"
}


#define RTCMEMORYSTART 65
#define MY_NTP_SERVER "de.pool.ntp.org"           
#define MY_TZ "CET-1CEST,M3.5.0/02,M10.5.0/03"   



#define BLYNK_PRINT Serial

#define ONE_WIRE_PIN 12
 
OneWire oneWire(ONE_WIRE_PIN);
DallasTemperature sensors(&oneWire);

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
    Serial.println(rtcMem.rainCount);
    Serial.println(rtcMem.year);
    Serial.println(rtcMem.y_day);
    if (wakeupReason == 0) {
      rtcMem.rainCount++;
      writeToRTCMemory();
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

    if (current_year == rtcMem.year && year_day == rtcMem.y_day) {
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
    sensors.begin();
    sensors.requestTemperatures();
    Serial.println(sensors.getTempCByIndex(0));
    Blynk.virtualWrite(V0, sensors.getTempCByIndex(0));
    Blynk.virtualWrite(V1, (double)rtcMem.rainCount * 0.25);

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
