#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include "mynet.h"
 
WiFiUDP UDP;
char packetBuffer[255];
 
void setup() {
 
  Serial.begin(115200);
  Serial.println();
 
//  WiFi.softAP(APSSID, APPASS);
  WiFi.begin(APSSID, APPASS);
  Serial.print("Connecting to ");
  Serial.println(APSSID);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }

  Serial.println("Success Wifi connect!!");
 
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  UDP.begin(localPort);
 
}

void loop() {
 
    UDP.beginPacket(udpReturnAddr, udpReturnPort);
    UDP.write("test now");
    if(UDP.endPacket()){
      Serial.println("send success");
    }else{
      Serial.println("send failure");
    }

    delay(1000);
}
