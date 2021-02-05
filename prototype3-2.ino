/*********************************************************
This example interfaces with the Adafruit MPR121 capacitive board: https://learn.adafruit.com/adafruit-mpr121-12-key-capacitive-touch-sensor-breakout-tutorial/overview
 * Reads all 0 pins and writes a 1 or 0 separated by commas
 * 
 * Based on the MPR121test.ino example file
**********************************************************/

#include <Wire.h>
#include "Adafruit_MPR121.h"

#ifndef _BV
#define _BV(bit) (1 << (bit)) 
#endif

// You can have up to 4 on one i2c bus but one is enough for testing!
Adafruit_MPR121 cap = Adafruit_MPR121();

// Keeps track of the last pins touched
// so we know when buttons are 'released'
uint16_t lasttouched = 0;
uint16_t currtouched = 0;

const int ledPinStart = 3;
const int ledPinEnd = 8;
const int ledCount = 6;
int currLed = 0;
int prevLed = 0;

unsigned long previousMillis = 0;        // will store last time LED was updated

// constants won't change:
const long interval = 500;           // interval at which to blink (milliseconds)
bool flashing = false;
const int controlPin = 0;

void setup() {
  Serial.begin(9600);

  while (!Serial) { // needed to keep leonardo/micro from starting too fast!
    delay(10);
  }

  for (int i=ledPinStart;i<=ledPinEnd; i++){
    pinMode(i, OUTPUT);
  }
  
  // Default address is 0x5A, if tied to 3.3V its 0x5B
  // If tied to SDA its 0x5C and if SCL then 0x5D
  if (!cap.begin(0x5A)) {
    Serial.println("MPR121 not found, check wiring?");
    while (1);
  }
  Serial.println("ready");
}

void flashLed() {
   unsigned long currentMillis = millis();

   if (currentMillis - previousMillis >= interval) {
    // save the last time you blinked the LED
    previousMillis = currentMillis;

    prevLed = currLed;
    
    if (currLed >= ledCount){
      currLed = -1;
      flashing = false;
    } 
    if (flashing){
      if (prevLed >= 0){
      Serial.print("off:");
      Serial.println(prevLed+ledPinStart);
      digitalWrite(prevLed+ledPinStart, LOW);
      }
      currLed = currLed +1;
      Serial.print("on:");
      Serial.println(currLed+ledPinStart);
      digitalWrite(currLed+ledPinStart, HIGH);
      
      
    }else {
      turnOffLed();
    }

    
  }
}

void turnOffLed() {
    flashing = false;
    for (int i=ledPinStart; i<=ledPinEnd; i++){
      digitalWrite(i, LOW);
    }
}

void loop() {
  // Get the currently touched pads
  currtouched = cap.touched();
  
  
  // it if *is* touched and *wasnt* touched before, alert!
  if ((currtouched & _BV(controlPin)) && !(lasttouched & _BV(controlPin)) ) {
    Serial.println("touched");
    turnOffLed();
  }
  // if it *was* touched and now *isnt*, alert!
  if (!(currtouched & _BV(controlPin)) && (lasttouched & _BV(controlPin)) ) {
      Serial.println("released");
      flashing = true;
      currLed = -1;
  }

  if (flashing){
      flashLed();
  }

  // reset our state
  lasttouched = currtouched;


  // put a delay so it isn't overwhelming
  delay(100);
}
