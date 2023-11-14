#define sensorPin 7
//#define BTN D1

void setup() {
  // put your setup code here, to run once:
  pinMode(sensorPin, INPUT);
  Serial.begin(9600);

}

void loop() {
  // put your main code here, to run repeatedly:
    int sensorValue = digitalRead(sensorPin);
    Serial.println(sensorValue);
    delay(50);
}
