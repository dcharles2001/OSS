
// Include NewPing Library
#include "NewPing.h"

// Hook up HC-SR04 with Trig to Arduino Pin 9, Echo to Arduino pin 10
#define TRIGGER_PIN 9
#define ECHO_PIN 10

#define sensorPin 7

// Maximum distance we want to ping for (in centimeters).
#define MAX_DISTANCE 400
#define OPEN_DISTANCE 30  

// NewPing setup of pins and maximum distance.
NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);

void setup() {
  pinMode(sensorPin, INPUT);
  Serial.begin(9600);
}

void loop() {
  float Dist = ((sonar.ping() / 2) * 0.0343);
  int sensorValue = digitalRead(sensorPin);
  if (Dist > OPEN_DISTANCE){
    Serial.println("Theifing Git");
    }else{
        Serial.print("Distance = ");
        Serial.print(Dist);
        Serial.println(" cm");
      }
  Serial.println(sensorValue);
  delay(100);
}
