

#include <FastLED.h>
#include <Wire.h>
#include "Adafruit_STMPE610.h"

#define NUM_LEDS 144
#define LED_COLS 16
#define LED_ROWS 9
#define DATA_PIN 2
#define XPOS_L   180
#define XPOS_H   650
#define YPOS_L   600
#define YPOS_H   2700
#define XAXIS    0
#define YAXIS    1

Adafruit_STMPE610 touch = Adafruit_STMPE610();

const int numReadings = 5;
int readings[2][numReadings];      // the readings from the touch sensor
int readIndex = 0;              // the index of the current reading
int xTotal = 0;                  // the running total
int yTotal = 0;                  // the running total
int xAverage = 0;                // the average
int yAverage = 0;                // the average

// Define the array of leds
CRGB leds[NUM_LEDS];

void fadeall() { for(int i = 0; i < NUM_LEDS; i++) { leds[i].nscale8(1000); } }

void setup(void) {
  
  Serial.begin(9600);
  FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS);  // GRB ordering is typical
  // initialize all the readings to 0:
  for (int thisReading = 0; thisReading < numReadings; thisReading++) {
    readings[XAXIS][thisReading] = 0;
    readings[YAXIS][thisReading] = 0;
  }
  if (! touch.begin()) {
    Serial.println("STMPE not found!");
    while(1);
  }
  Serial.println("Waiting for touch sense");
}

void loop(void) {
  static uint8_t hue = 0;
  uint16_t x, y;
  uint8_t z;
  int xpos, ypos;
  //FastLED.clear(); FastLED.show();
  if (touch.touched()) {
    while (! touch.bufferEmpty()) {
      //Serial.print("readIndex: "); Serial.print(readIndex); Serial.print("\t");
      xTotal = xTotal - readings[XAXIS][readIndex]; // subtract the last reading:
      yTotal = yTotal - readings[YAXIS][readIndex]; // subtract the last reading:
      touch.readData(&x, &y, &z); // read x & y & z;
      //Serial.print("x: "); Serial.print(x); Serial.print("\t");
      readings[XAXIS][readIndex] = x;
      readings[YAXIS][readIndex] = y;
      xTotal = xTotal + readings[XAXIS][readIndex]; // add the reading to the total:
      yTotal = yTotal + readings[YAXIS][readIndex]; // add the reading to the total:
      //Serial.print("total: "); Serial.print(total); Serial.print("\t");
      readIndex++; // advance to the next position in the array:
      if (readIndex >= numReadings) {
          readIndex = 0; // ...wrap around to the beginning:
      }      
      xAverage = xTotal / numReadings; // calculate the average:
      yAverage = yTotal / numReadings; // calculate the average:
      //Serial.print("xAverage: ");Serial.println(xAverage); Serial.print("\t");
      xpos = map(constrain(xAverage,XPOS_L,XPOS_H),XPOS_L,XPOS_H,1,LED_COLS);
      ypos = map(constrain(yAverage,YPOS_L,YPOS_H),YPOS_L,YPOS_H,LED_ROWS,1); 
      //Serial.print(x); Serial.print("\t"); Serial.println(y); //Serial.print("\t"); //Serial.println(z);

      //FastLED.clear();
      if(ypos%2==0){
        leds[ypos*LED_COLS - xpos] = CHSV(hue++, 255, 255); //CRGB::Red;
      }
      else{
        leds[ypos*LED_COLS - LED_COLS + xpos - 1] = CHSV(hue++, 255, 255); //CRGB::Red;
      }
      FastLED.show();
      fadeall();
    }
    touch.writeRegister8(STMPE_INT_STA, 0xFF); // reset all ints, in this example unneeded depending in use
    
  }
  else{
    FastLED.clear(); FastLED.show();
  }
  
  //delay(10);
}
