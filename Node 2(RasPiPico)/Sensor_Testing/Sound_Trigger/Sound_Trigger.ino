#define sensorPin A0


void setup() {
  // put your setup code here, to run once:
  pinMode(sensorPin, INPUT);
  Serial.begin(9600);

}

void loop() {
  // put your main code here, to run repeatedly:
    int sensorValue = analogRead(sensorPin);
    Serial.println(sensorValue);
    delay(200);
}
