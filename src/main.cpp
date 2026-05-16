
#include "esp32-hal.h"
#include <Arduino.h>
#include <U8g2lib.h>
#include <Wire.h>
#include <esp_now.h>
#include <WiFi.h>
#define x 0
#define y 1
//objects:
U8G2_SH1106_128X64_NONAME_F_HW_I2C screen(U8G2_R0, U8X8_PIN_NONE);
esp_now_peer_info_t peerInfo;
//variables:
struct Payload{
  double amplitude; //0 - 35
  double direction; //direction in angles
  double spin;      //positive clockwise, negative counter, keep between -20 and 20
  double level;        //from -20 to 20 (lower numbber means higher robot)

};
Payload data;
const byte address[6] = "00001";
uint8_t receiverAddress[] = {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF};
//constants:
int joyIndex[][2] = {{0,1},{2,3}};
int cursorPosX = 0;
int cursorPosY = 10;
bool connectionStatus = 0;
//functions
int get_joy_vals(int stick,int dimension,bool inverted){
  int val = analogRead(joyIndex[stick][dimension]);
  double valProc = ((double)val / 4095.0) * 2.0 - 1.0; 
  if(inverted){
    return -valProc;
  }
  else{
    return valProc;
  }
}
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}


void setup() {
  //init screen:
  screen.begin();
  screen.setFont(u8g2_font_t0_11_tr);
  screen.setContrast(255);
  //init inputs:
  for(int i = 0; i < 2; i++){
    for(int v = 0; v< 2; v++){
      pinMode(joyIndex[i][v],INPUT);
    }
  }
  //init Wifi
  WiFi.mode(WIFI_STA);
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  esp_now_register_send_cb(OnDataSent);
  memcpy(peerInfo.peer_addr, receiverAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }
}

void loop() {
  //get joystick vals 
  //proc joystick vals
  //send joystick vals
  //print thing to screen
  

  //pass joystick values to pack
  data.amplitude = map(get_joy_vals(0,x,0),-1,1,0,35);
  data.direction = atan2(get_joy_vals(0,y,0), get_joy_vals(0,x,0));
  data.spin = map(get_joy_vals(1,y,1), -1, 1,-20,20);
  data.level = map(get_joy_vals(1,x,1), -1, 1, 20, -20);

  //send pack via esp now
  esp_now_send(receiverAddress, (uint8_t *) &data, sizeof(data));


  //print stats to screen
  cursorPosY = 10;
  cursorPosX = 0;
  screen.clearBuffer();
  cursorPosX += screen.drawStr(0, cursorPosY, "Power: ");
  cursorPosX += screen.drawStr(cursorPosX, cursorPosY, String((int)data.amplitude).c_str());
  cursorPosY += 10;
  cursorPosX = 0;
  cursorPosX += screen.drawStr(cursorPosX, cursorPosY, "Direction: ");
  cursorPosX += screen.drawStr(cursorPosX, cursorPosY, String((int)data.direction).c_str());
  cursorPosY += 10;
  cursorPosX = 0;
  cursorPosX += screen.drawStr(cursorPosX, cursorPosY, "Connection: ");
  cursorPosX += screen.drawStr(cursorPosX, cursorPosY, String((int)connectionStatus).c_str());
  screen.sendBuffer();
  delay(1000);
}