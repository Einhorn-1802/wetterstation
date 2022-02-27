#include <ESP8266WiFi.h>        // Include the Wi-Fi library
#include <NTPClient.h>
#include <WiFiUdp.h>
#include "wlanData.h"
#include "myTime.h"
#include "utcMemory.h"
#include <OneWire.h>
#include <DallasTemperature.h>
#include <BlynkSimpleEsp8266.h>

#define RTCMEMORYSTART 65


extern "C" {
#include "user_interface.h"
}

#define BLYNK_PRINT Serial

#define ONE_WIRE_PIN 12
 
OneWire oneWire(ONE_WIRE_PIN);
DallasTemperature sensors(&oneWire);

char auth[] = BLYNK_AUTH_TOKEN;
#define BLYNK_FIRMWARE_VERSION "0.0.1"

typedef struct {
  int rainCount;
  int year;
  int y_day;
} rtcStore;

rtcStore rtcMem;
unsigned long epochTime;

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
    }    
    Blynk.begin(auth, ssid, password);
    pinMode(D4, OUTPUT);
    timeClient.begin();
    timeClient.update();
    epochTime = timeClient.getEpochTime();
    struct tm *ptm = gmtime ((time_t *)&epochTime);
    int current_year = ptm->tm_year + 1900;
    int year_day = ptm-> tm_yday;
    int day_hour = ptm-> tm_hour;
    Serial.print("Epoch Time: ");
    Serial.println(epochTime);
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
    delay(100);
//    ESP.deepSleep(1e6*30);
    ESP.deepSleep(0);

}

void loop() {

}
