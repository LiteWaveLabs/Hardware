/*
  Blink
  Turns on an LED on for one second, then off for one second, repeatedly.

  Most Arduinos have an on-board LED you can control. On the Uno and
  Leonardo, it is attached to digital pin 13. If you're unsure what
  pin the on-board LED is connected to on your Arduino model, check
  the documentation at http://www.arduino.cc

  This example code is in the public domain.

  modified 8 May 2014
  by Scott Fitzgerald
 */
#include <Wire.h>
#include <Adafruit_ADS1015.h>

// Adafruit_ADS1115 ads;  /* Use this for the 16-bit version */
Adafruit_ADS1015 ads;     /* Use thi for the 12-bit version */
#include <Adafruit_NeoPixel.h>

#define GPIO0     2
#define LED1      6
#define USBenable 5
#define LED2      7
#define LED3      8
#define Piezo     9
#define PiezoFET  10
#define Charge    11 //active LOW
#define Trigger   12
#define RGBLED    A1
#define TabletFET A2
#define Tablet    13


Adafruit_NeoPixel strip = Adafruit_NeoPixel(35, RGBLED, NEO_GRB + NEO_KHZ800);


char command;
boolean stringComplete;
String argument;

void setup() {
  pinMode(TabletFET, OUTPUT);
  digitalWrite(TabletFET, LOW);
  
  pinMode(RGBLED, OUTPUT);
  pinMode(Trigger, OUTPUT);
  strip.begin();

  pinMode(Piezo, OUTPUT);
  pinMode(PiezoFET, OUTPUT);

  pinMode(USBenable, OUTPUT);
  digitalWrite(USBenable, HIGH);
  
  Serial.begin(57600);
  ads.setGain(GAIN_TWO);
  ads.begin();

  rainbowCycle(10);

  Serial.println("start");
  
  digitalWrite(TabletFET, HIGH);
  delay(3000);
  pinMode(Tablet, OUTPUT);
  digitalWrite(Tablet, LOW);
  delay(5000);
  pinMode(Tablet, INPUT);
  
}


void loop() {

  if (Serial.available()) {
    command  = Serial.readStringUntil('+').charAt(0);;
    Serial.read();
    argument = Serial.readStringUntil('\n');
    Serial.read();

    stringComplete = true;
  }

  if (stringComplete) {
    stringComplete = false;
    switch (command) {
      case 'r': //rainbow
        rainbowCycle(argument.toInt());
        break;
      case 'f': //specific pixel
        strip.setPixelColor(argument.substring(0, 2).toInt(), strip.Color(argument.substring(2, 5).toInt(), argument.substring(5, 8).toInt(), argument.substring(8, 11).toInt()));
        strip.show();
        break;
        
      case 'v': //cap voltage
        Serial.println("v=" + capVoltage());
        break;

      case 'b': //battery voltage
        Serial.println("b=" + String(batVoltage()));
        break;

      case 't': //trigger
        trigger();
        break;

      case 'p': //trigger
        delay(3000);
        digitalWrite(Piezo, HIGH);
        digitalWrite(PiezoFET, HIGH);
        delay(6000);
        digitalWrite(Piezo, LOW);
        digitalWrite(PiezoFET, LOW);
        break;

      case 'c': //charge
        charge(argument.toInt());
        break;
      default:
        Serial.println("Fatal Error!");
    }
  }
    

}

void clearWS() {
  for (int i = 0; i <= strip.numPixels(); i++) {
    strip.setPixelColor(i, strip.Color(0, 0, 0));
  }
  strip.show();
}

void trigger() {
  digitalWrite(Trigger, HIGH);
  delay(200);
  digitalWrite(Trigger, LOW);
}

int capVoltage() {
  int volts;
  volts = (ads.readADC_SingleEnded(1) / 1000.00) * 267.60;
  return volts;
}

float batVoltage() {
  float volts;
  volts = (ads.readADC_SingleEnded(0) / 1000.00) * 2.00;
  return volts;
}

void charge(int voltage) {
  strip.show();
  pinMode(Charge, OUTPUT);
  digitalWrite(Charge, LOW);
  int readout = 0;
  int count = 0;
  
  while (voltage >= readout/* && !Serial.available()*/) {
    for (int i = 0; i <= strip.numPixels(); i++) {

      if(i % 4 == 0) {
        readout = capVoltage();
        //mapCharge(voltage, readout);
        Serial.println("c=" + String(readout));
        count = map(readout, 0, voltage, 0, strip.numPixels());
      }
  
  
      for (int a = 0; a <= count; a++) {
        strip.setPixelColor(a, strip.Color(0, 200, 0));
      }

      for (int a = count + 1; a <= strip.numPixels(); a++) {
        strip.setPixelColor(a, strip.Color(0, 0, 0));
      }

    
      strip.setPixelColor(i, strip.Color(255, 0, 0));
      if (i < 18)
        strip.setPixelColor(i+17, strip.Color(255, 0, 0));

      else
        strip.setPixelColor(i-17, strip.Color(255, 0, 0));
          
      strip.show();
      delay(25);
    }
  }
  for (int i = 0; i <= strip.numPixels(); i++) {
    strip.setPixelColor(i, strip.Color(255, 0, 0));
  }
  digitalWrite(Piezo, HIGH);
  digitalWrite(PiezoFET, HIGH);
  strip.show();
  delay(1000);
        
  clearWS();
  delay(1000);

  for (int i = 0; i <= strip.numPixels(); i++) {
    strip.setPixelColor(i, strip.Color(255, 0, 0));
  }
  strip.show();

  delay(3000);
  
  trigger();

  digitalWrite(Piezo, LOW);
  digitalWrite(PiezoFET, LOW);

  clearWS();
  pinMode(Charge, INPUT);
  delay(500);
  Serial.println("!");
}

void mapCharge(int maxVoltage, int actualVoltage) {
  int count = map(actualVoltage, 0, maxVoltage, 0, strip.numPixels());
  for (int i = 1; i <= strip.numPixels(); i++) {
  
  
    for (int a = 0; a <= count; a++) {
      strip.setPixelColor(a, strip.Color(0, 200, 0));
    }

    for (int a = count + 1; a <= strip.numPixels(); a++) {
      strip.setPixelColor(a, strip.Color(0, 0, 0));
    }

    
    strip.setPixelColor(i, strip.Color(255, 0, 0));
    strip.show();
    delay(3);
  }
}

uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}

void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    delay(wait);
  }
  clearWS();
}

