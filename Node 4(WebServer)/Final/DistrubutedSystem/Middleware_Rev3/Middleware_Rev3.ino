#include "Middleware_OS.h"

Middleware_OS MOS;

void setup(){
  MOS.Setup();
  Serial.begin(9600);
}

void loop(){
  String Message = "Hello World!";
  MOS.Send(Message, 01);

  // String Response = MOS.Receive();
  // Serial.println(Response);
}