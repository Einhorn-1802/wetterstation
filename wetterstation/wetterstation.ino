#include <ESP8266WiFi.h>        // Include the Wi-Fi library
#include <NTPClient.h>
#include <WiFiUdp.h>
#include "wlanData.h"
#include "myTime.h"

void setup() {
  // put your setup code here, to run once:
    Serial.begin(115200);         // Start the Serial communication to send messages to the computer
    connectWifi();
    timeClient.begin();
    
    
    

}

void connectWifi() {
   Serial.println('\n');
  
   WiFi.begin(ssid, password);             // Connect to the network
   Serial.print("Connecting to ");
   Serial.print(ssid); Serial.println(" ...");

   int i = 0;
   while (WiFi.status() != WL_CONNECTED) { // Wait for the Wi-Fi to connect
   delay(1000);
   Serial.print(++i); Serial.print(' ');
   }

   Serial.println('\n');
   Serial.println("Connection established!");  
   Serial.print("IP address:\t");
   Serial.println(WiFi.localIP());         // Send the IP address of the ESP8266 to the computer

};


void loop() {
  // put your main code here, to run repeatedly:
    timeClient.update();

  Serial.print(daysOfTheWeek[timeClient.getDay()]);
  Serial.print(", ");
  Serial.print(timeClient.getHours());
  Serial.print(":");
  Serial.print(timeClient.getMinutes());
  Serial.print(":");
  Serial.println(timeClient.getSeconds());
  Serial.println(timeClient.getFormattedTime());

  delay(1000);

}
