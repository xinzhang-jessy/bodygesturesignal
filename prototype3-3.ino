
/* DIGF Body Centric
 * Jessy（Xin Zhang）
 * 
 * 
 * This example interfaces with the Adafruit MPR121 capacitive board: https://learn.adafruit.com/adafruit-mpr121-12-key-capacitive-touch-sensor-breakout-tutorial/overview
 * Reads 0-1 pins and writes a 1 or 0 separated by commas
 * 
 * Based on the MPR121test.ino example file
 * 
  */

#include <LedControl.h>

#include <Wire.h>
#include "Adafruit_MPR121.h"

#ifndef _BV
#define _BV(bit) (1 << (bit)) 
#endif


// You can have up to 4 on one i2c bus but one is enough for testing!
Adafruit_MPR121 cap = Adafruit_MPR121();
const long interval = 1000;           // interval at which to blink (milliseconds)

// Keeps track of the last pins touched
// so we know when buttons are 'released'
uint16_t lasttouched = 0;
uint16_t currtouched = 0;

int DIN = 10;
int CS =  11;
int CLK = 12;

byte all[8] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
byte heart1[8] = {0x00, 0x66, 0xFF, 0xFF, 0x7E, 0x3C, 0x18, 0x00};
byte heart2[8] = {0x00, 0x00, 0x24, 0x7E, 0x3C, 0x18, 0x00, 0x00};
byte empty[8] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
unsigned long previousMillis = 0;        // will store last time LED was updated

LedControl lc = LedControl(DIN, CLK, CS, 2);

bool flashing = false;
const int controlPin1 = 0;
const int controlPin2 = 1;
int heartStatus = 1;

void setup() {
  for (int i = 0; i < 2; i++) {
    lc.shutdown(i, false);      //启动时，MAX72XX处于省电模式
    lc.setIntensity(i, 1);      //将亮度设置为最大值
    lc.clearDisplay(i);//清除显示
    Serial.print("yo");
  }
    Serial.begin(9600);

  while (!Serial) { // needed to keep leonardo/micro from starting too fast!
    delay(10);
  }
  
  // Default address is 0x5A, if tied to 3.3V its 0x5B
  // If tied to SDA its 0x5C and if SCL then 0x5D
  if (!cap.begin(0x5A)) {
    Serial.println("MPR121 not found, check wiring?");
    while (1);
  }
  Serial.println("ready");
}

void flashHeart(){
   unsigned long currentMillis = millis();

   if (currentMillis - previousMillis >= interval) {
    // save the last time you blinked the LED
    previousMillis = currentMillis;

    if (heartStatus == 1){
      heartStatus = 2;
    } else {
      heartStatus = 1;
    }

    if (heartStatus == 1){
      printHeart1();
    } else {
      printHeart2();
    }
  }
}

void printHeart1(){
  Serial.println("print heart1");
  printByte(0, heart1);
  printByte(1, heart1);
}

void printHeart2(){
  Serial.println("print heart2");
  printByte(0, heart2);
  printByte(1, heart2);
}

void stopFlashHeart(){
  lc.clearDisplay(0);
  lc.clearDisplay(1); 
}

void loop() {
   // Get the currently touched pads
  currtouched = cap.touched();

  bool pinOn1 = bool(currtouched & _BV(controlPin1));
  bool pinOn2 = bool(currtouched & _BV(controlPin2));
  Serial.println(pinOn1);
  Serial.println(pinOn2);
  
    // it if *is* touched and *wasnt* touched before, alert!
    if (!(pinOn1 && pinOn2) && (pinOn1 || pinOn2)  ){
      if (!flashing) Serial.println("touched");
      flashing = true;
    } else {
      if (flashing) Serial.println("released");
      flashing = false;
    }
  
    
  if (flashing){
      flashHeart();
  } else {
    stopFlashHeart();
  }

  // reset our state
  lasttouched = currtouched;


  // put a delay so it isn't overwhelming
  delay(100);
}

//点阵显示函数
void printByte(int num, byte character []) {
  int i = 0;
  for (i = 0; i < 16; i++) {
    lc.setRow(num, i, character[i]);
  }
}
