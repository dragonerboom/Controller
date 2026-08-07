#include <Arduino.h>
#include <cmath>
#include <esp_now.h>
#include <WiFi.h>
#define x 0
#define y 1
//##################################OBJECTS##################################
esp_now_peer_info_t peerInfo;
//##################################PAYLOAD##################################
struct Payload{
  double amplitude; //0 - 35
  double direction; //direction in angles
  double spin;      //positive clockwise, negative counter, keep between -20 and 20
  double level;        //from -20 to 20 (lower numbber means higher robot)

};
Payload data;
//##################################VARIABLES##################################
const byte address[6] = "00001";
uint8_t receiverAddress[] = {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF};
bool connectionStatus = 0;
double stickAX;
double stickAY;
double stickBX;
double stickBY;


void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

double deadzone(double stick){
  if (fabs(stick) < 0.2){return 0;}
  else{return stick;}
}

void setup() {
  //init screen:
  
  //init inputs:
  
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
  //init ads
  
}

void loop() {
  //get input
  stickAX = (float)(analogRead(A0) - 2048) / 2048;
  stickAY = (float)(analogRead(A1) - 2048) / 2048;
  stickBX = (float)(analogRead(A2) - 2048) / 2048;
  stickBY = (float)(analogRead(A3) - 2048) / 2048;
  //deadzone
  stickAX = deadzone(stickAX); 
  stickAY = deadzone(stickAY);
  stickBX = deadzone(stickBX);
  stickBY = deadzone(stickBY);
  
  //send data
  esp_now_send(receiverAddress, (uint8_t *) &data, sizeof(data));


  
  
  
  delay(1000);
}

