#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <EEPROM.h>
#include <ESP8266WebServer.h>
#include "mynet.h"
#include "board.h"

extern "C" {
#include "user_interface.h"
}

#define DEBUG

WiFiUDP UDP;
ESP8266WebServer server(80);
char stcRcvBuf[STC_RECEIVE_BUFF_SIZE];
char buffSPCUsr[5] = {0};

unsigned long tim = 0;
int pack_size = 0;
int stcMode = 0;
int voltage = 0;

//ip config
IPAddress STCHostIP(192, 168, 4, 1);
IPAddress STCClientIP(192, 168, 4, 2);

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
  int j = 0;
  for (i; EEPROM.read(i) != 0; i++) {
    pass[j] = (char)EEPROM.read(i);
    j++;
  }

  Serial.print("Wifi SSID : ");
  Serial.println(ssid);
//  Serial.println(pass);

}

void STCClientMode() {

//  toggleLEDG();

#ifdef DEBUG
  Serial.println("STC Client Mode...");
#endif

  //setup
  WiFi.mode(WIFI_STA);
  WiFi.begin(STC_HOST_SSID, STC_HOST_PASS);
  WiFi.config(STCClientIP, WiFi.gatewayIP(), WiFi.subnetMask());
  UDP.begin(STCClientPort);

  tim = millis();
  while ((millis() - tim) < modeTime_ms * 2) {
    //  while (1) {

    //Wifiの接続が確立している時のみ処理
    if (WiFi.status() == WL_CONNECTED) {

      Serial.print("RSSI : ");
      Serial.println(WiFi.RSSI());

      if (WiFi.RSSI() > valRSSI) {
        digitalWrite(LEDB, HIGH);
        Serial.println("Connecting Host");

        //送信処理
        UDP.beginPacket(STCHostIp, STCHostPort);
        UDP.write(ownerID);
        UDP.endPacket();
        Serial.println("Send to Host");
      }



      //受信処理
      pack_size = UDP.parsePacket();
      if (pack_size > 0) {
        if (pack_size > STC_RECEIVE_BUFF_SIZE) {
          pack_size = STC_RECEIVE_BUFF_SIZE;
        }
        UDP.read(stcRcvBuf, pack_size);
        UDP.flush();
#ifdef DEBUG
        Serial.println(stcRcvBuf);
#endif
      }


    } else {
      digitalWrite(LEDB, LOW);
    }
    delay(5);
  }

}

void STCHostMode() {

//  toggleLEDG();

#ifdef DEBUG
  Serial.println("STC Host Mode...");
#endif

  //setup
  WiFi.mode(WIFI_AP);
  WiFi.softAP(STC_HOST_SSID, STC_HOST_PASS);
  WiFi.config(STCHostIP, WiFi.gatewayIP(), WiFi.subnetMask());
  UDP.begin(STCHostPort); // setting UDP port

  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);

  tim = millis();
  while ((millis() - tim) < modeTime_ms * 2) {
    //  while (1) {

    //受信割り込み
    if ((pack_size = UDP.parsePacket()) > 0) {

      //受信処理
      if (pack_size > STC_RECEIVE_BUFF_SIZE) {
        pack_size = STC_RECEIVE_BUFF_SIZE;
      }
      UDP.read(stcRcvBuf, pack_size);
      UDP.flush();

#ifdef DEBUG
      Serial.println(stcRcvBuf);
#endif

      //送信処理
      UDP.beginPacket(STCClientIp, STCClientPort);
      UDP.write(ownerID);
      UDP.endPacket();

    }
    delay(5);
  }

}

void sendToServerMode() {

//  toggleLEDG();

#ifdef DEBUG
  Serial.println("Server send Mode...");
#endif

  //setup
  WiFi.mode(WIFI_STA);
  //  WiFi.begin(APSSID, APPASS);
  WiFi.begin(ssid, pass);
  WiFi.config(STCClientIP, WiFi.gatewayIP(), WiFi.subnetMask());
  UDP.begin(STCClientPort);

  tim = millis();
  while ((millis() - tim) < modeTime_ms) {

    //Wifiの接続が確立している時のみ処理
    if (WiFi.status() == WL_CONNECTED) {
      UDP.beginPacket(ServerAddr, ServerPort);
      char send_data[6];
      sprintf(send_data, "%c,%c,0,0,0,0", *ownerID, stcRcvBuf[0]);
      Serial.print("data : ");
      Serial.println(send_data);
      UDP.write(send_data);
      UDP.endPacket();
      break;
    }

    delay(5);
  }

  WiFi.disconnect();

}

void toggleLEDG() {
  if (digitalRead(LEDG)) {
    digitalWrite(LEDG, LOW);
  } else {
    digitalWrite(LEDG, HIGH);
  }
}

void timer0_ISR (void) {
  toggleLEDG();
  timer0_write(ESP.getCycleCount() + 160000000L); // 80MHz == 1sec
}

void setup() {

  //serial setup
  Serial.begin(115200);
  Serial.println();

  /*
     board setup
  */

  //led
  pinMode(LEDR, OUTPUT);
  pinMode(LEDG, OUTPUT);
  pinMode(LEDB, OUTPUT);
  digitalWrite(LEDR, LOW);
  digitalWrite(LEDG, LOW);
  digitalWrite(LEDB, LOW);

  noInterrupts();
  timer0_isr_init();
  timer0_attachInterrupt(timer0_ISR);
  timer0_write(ESP.getCycleCount() + 160000000L); // 80MHz == 1sec
  interrupts();


  /*
     Wifiの初期設定
  */

  EEPROM.begin(200);

  bool is_first_boot = false;
  if (EEPROM.read(0) == 255) {
    is_first_boot = true;
  }

  if (is_first_boot) {
    WiFi.mode(WIFI_AP);
    WiFi.softAP("Tunagalbum", "tunatuna");
    WiFi.config(STCHostIP, WiFi.gatewayIP(), WiFi.subnetMask());
    server.on("/", HTTP_GET, handleRootGet);
    server.on("/", HTTP_POST, handleRootPost);
    server.begin();
    Serial.println("It is first boot.\nHTTP server started.");
    while (1) {
      server.handleClient();
    }
  } else {
    getSsidPass(ssid, pass);
  }

  wifi_set_sleep_type(MODEM_SLEEP_T);
}

void sleepMode(){
    Serial.println("/*------- Battery is lower -------*/");
    digitalWrite(LEDR,HIGH);
    ESP.deepSleep(1000);
    delay(1000);  
}

void loop() {

  if(analogRead(A0) < 690){
    sleepMode();
  }

  stcMode++;
  switch (stcMode) {
    case 1:
      STCClientMode();
      break;
    case 2:
      STCHostMode();
      break;
    case 3:
      sendToServerMode();
      stcMode = 0;
      break;
    default:
      stcMode = 0;
      break;
  }
  delay(100);
}
