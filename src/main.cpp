#include <Arduino.h>
#include <RF24.h>
#include <RF24_config.h>
#include <nRF24L01.h>
#include <printf.h>
#include <LiquidCrystal_I2C.h>
#define x 0
#define y 1
//objects:
RF24 radio (7,8);
LiquidCrystal_I2C lcd(0x27,20,4);
//variables:
struct Payload{
  double amplitude; //0 - 35
  double direction; //direction in angles
  double spin;      //positive clockwise, negative counter, keep between -20 and 20
  double level;        //from -20 to 20 (lower numbber means higher robot)

};
Payload data;
const byte address[6] = "00001";
//constants:
int joyIndex[][2] = {{1,2},{3,4}};

int get_joy_vals(int stick,int dimension,bool inverted){
  int val = analogRead(joyIndex[stick][dimension]);
  double valProc = map(val,0,4095,-1,1);
  if(inverted){
    return -valProc;
  }
  else{
    return valProc;
  }
}


void setup() {
  //init radio:
  radio.begin();
  radio.openWritingPipe(address);
  radio.stopListening();
  radio.setPALevel(RF24_PA_MIN);
  //init screen:
  lcd.init();
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.clear();
  lcd.print("init proc");
  //init inputs:
  for(int i = 0; i < 2; i++){
    for(int v = 0; v< 2; v++){
      pinMode(joyIndex[i][v],INPUT);
    }
  }
  lcd.clear();
  lcd.print("finished setup");
}

void loop() {
  //get joystick vals 
  //proc joystick vals
  //send joystick vals
  //print thing to screen

  data.amplitude = map(get_joy_vals(0,x,0),-1,1,0,35);
  data.direction = atan2(get_joy_vals(0,y,0), get_joy_vals(0,x,0));
  data.spin = map(get_joy_vals(1,y,1), -1, 1,-20,20);
  data.level = map(get_joy_vals(1,x,1), -1, 1, 20, -20);
  radio.write(&data, sizeof(data));

}