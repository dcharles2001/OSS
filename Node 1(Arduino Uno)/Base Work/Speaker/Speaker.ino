#define Alarm 11

int i = 0;
void setup() {
  // put your setup code here, to run once:
    pinMode(Alarm, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  for(int i = 0; i<3; i++){
    analogWrite(Alarm,230);
    delay(500);
    analogWrite(Alarm,0);
    delay(1500);
  }
 while(true){
    analogWrite(Alarm,0);
  }   
}
