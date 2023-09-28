#define Input 10
#define RLED 13
#define YLED 12
void setup() {
  // put your setup code here, to run once:
  pinMode(Input, INPUT);
  pinMode(RLED, OUTPUT);
  pinMode(YLED, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  int Data = digitalRead(Input);
  Serial.println(Data);
  if (Data == 1){
     digitalWrite(RLED,HIGH);
  }else{
    digitalWrite(RLED,LOW);
  }
}
