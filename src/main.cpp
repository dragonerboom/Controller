#include <Arduino.h>
#include <U8g2lib.h>
#include <Wire.h>
#define x 0
#define y 1
//objects:
U8G2_SH1106_128X64_NONAME_F_HW_I2C screen(U8G2_R0, U8X8_PIN_NONE);
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
  //display sstick info on screen
  screen.clearBuffer();
  screen.drawCircle(64, 32, 28);
  screen.drawLine(64, 32, map(get_joy_vals(0, x, 0),-1,1,64-28,64+28),map(get_joy_vals(0, y, 0),-1,1,64-28,64+28));
  screen.sendBuffer();
  //demo text

  /*
  screen.clearBuffer();
  screen.setCursor(0, 10);                 
  screen.print("I WORK !!!!");
  screen.setCursor(0,19);
  screen.print(":D");
  screen.sendBuffer();
  delay(1000);
  */
}