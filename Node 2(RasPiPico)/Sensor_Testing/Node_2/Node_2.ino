
// Include NewPing Library
#include <NewPingESP8266.h>

// Hook up HC-SR04 with Trig to Arduino Pin 9, Echo to Arduino pin 10
#define TRIGGER_PIN 12 //D6
#define ECHO_PIN 14  //D5
#define sensorPin 13 //D7

// Maximum distance we want to ping for (in centimeters).
#define MAX_DISTANCE 400
#define OPEN_DISTANCE 30  

float Dist;
int Sound;
int Alarm = 0;

// NewPing setup of pins and maximum distance.
NewPingESP8266 sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);

void setup() {
  pinMode(sensorPin, INPUT);
  Serial.begin(9600);
}

void loop() {
  Dist = ((sonar.ping() / 2) * 0.0343);
  int sensorValue = digitalRead(sensorPin);
  if (Dist > OPEN_DISTANCE){
    Alarm = 1;
    Serial.println("Safe Open");
    }else{
        Serial.print("Distance = ");
        Serial.print(Dist);
        Serial.println(" cm");
      }
  if (sensorValue == 1){
    Serial.print("Noise Detected");
    Alarm = 1;
    }
  Serial.println(sensorValue);
}
