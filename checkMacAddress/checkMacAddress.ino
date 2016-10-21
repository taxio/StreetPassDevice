#include <ESP8266WiFi.h>

void setup() {
  Serial.begin(115200);

  byte macAddr[6];
  WiFi.macAddress(macAddr);
  char buf[30];
  sprintf(buf,"\n%02X:%02X:%02X:%02X:%02X:%02X",
  macAddr[0],
  macAddr[1],
  macAddr[2],
  macAddr[3],
  macAddr[4],
  macAddr[5]);
  Serial.print(buf);
}

void loop() {
  delay(1000);
}
