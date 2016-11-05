#include <ESP8266WiFi.h>
#include <EEPROM.h>
#include <ESP8266WebServer.h>

bool is_first_boot = false;

ESP8266WebServer server(80);
IPAddress myIP(192, 168, 4, 1);

void handleRootGet() {
  String html = "";
  html += "<h1>WiFi Settings</h1>";
  html += "<form method='post'>";
  html += "  <input type='text' name='ssid' placeholder='ssid'><br>";
  html += "  <input type='text' name='pass' placeholder='pass'><br>";
  html += "  <input type='submit'><br>";
  html += "</form>";
  server.send(200, "text/html", html);
}

void handleRootPost() {
  String ssid = server.arg("ssid");
  String pass = server.arg("pass");

  Serial.println(ssid);
  Serial.println(pass);

  String ssid_pass = ssid + "," + pass;
  Serial.println(ssid_pass.length());
  byte write_data[200] = {0};
  ssid_pass.getBytes(write_data, ssid_pass.length() + 1);
  int i;
  for (i = 0; write_data[i] != 0; i++) {
    Serial.print((char)write_data[i]);
    EEPROM.write(i, write_data[i]);
    if (i >= 200) {
      break;
    }
  }
  EEPROM.write(i, 0);
  EEPROM.commit();
  Serial.println();


  for (i = 0; i < 200; i++) {
    if (!EEPROM.read(i)) {
      break;
    }
    Serial.print(EEPROM.read(i));
  }
  Serial.println();

  String html = "";
  html += "<h1>WiFi Settings</h1>";
  html += ssid + "<br>";
  html += pass + "<br>";
  server.send(200, "text/html", html);
}

void getSsidPass(char *ssid, char *pass) {

  int i;
  for (i = 0; (char)EEPROM.read(i) != ','; i++) {
    ssid[i] = (char)EEPROM.read(i);
  }
  i++;
  int j=0;
  for (i; EEPROM.read(i) != 0; i++) {
    pass[j] = (char)EEPROM.read(i);
    j++;
  }

  Serial.println(ssid);
  Serial.println(pass);

}

void setup() {
  Serial.begin(115200);
  Serial.println();

  EEPROM.begin(200);

  if (EEPROM.read(0) == 255) {
    is_first_boot = true;
  }

  if (is_first_boot) {
    WiFi.mode(WIFI_AP);
    WiFi.softAP("Tunagalbum", "tunatuna");
    WiFi.config(myIP, WiFi.gatewayIP(), WiFi.subnetMask());
    server.on("/", HTTP_GET, handleRootGet);
    server.on("/", HTTP_POST, handleRootPost);
    server.begin();
    Serial.println("HTTP server started.");
    while (1) {
      server.handleClient();
    }
  } else {
    WiFi.mode(WIFI_STA);
    char ssid[100]={0}, pass[100]={0};
    getSsidPass(ssid, pass);
    WiFi.begin(ssid,pass);
    WiFi.config(myIP, WiFi.gatewayIP(), WiFi.subnetMask());
    while(WiFi.status() != WL_CONNECTED){
      Serial.print(".");
      delay(500);
    }
    Serial.println("connented!");
  }
}

void loop() {
}
