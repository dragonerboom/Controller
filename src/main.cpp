#include "esp32-hal.h"
#include <Arduino.h>
#include <TFT_eSPI.h>
#include <Wire.h>
#include <Adafruit_ADS1X15.h>
#include <cmath>
#include <esp_now.h>
#include <WiFi.h>
#define x 0
#define y 1
//##################################OBJECTS##################################
esp_now_peer_info_t peerInfo;
Adafruit_ADS1115 ads;
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


void setup() {
  //init screen:
  
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
  //init ads
  ads.begin()
  ads.setGain(GAIN_TWOTHIRDS);
}

void loop() {

  stickAX = map(ads.readADC_SingleEnded(0),0,26400,-1,1);
  stickAY = map(ads.readADC_SingleEnded(1),0,26400,-1,1);
  stickBX = map(ads.readADC_SingleEnded(2),0,26400,-1,1);
  stickBY = map(ads.readADC_SingleEnded(3),0,26400,-1,1);

  data.amplitude = stickAX;
  data.direction = atan2(stickAY, stickAX);
  data.spin = stickBY; 
  data.level= stickBX;

  esp_now_send(receiverAddress, (uint8_t *) &data, sizeof(data));


  //print stats to screen
  
  
  delay(1000);
}