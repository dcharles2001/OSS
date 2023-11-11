#include "Middleware_OS.h"

RF24 radio(D4, D2);
RF24Network network(radio);

void Middleware_OS::Setup(){
  SPI.begin();
  radio.begin();
  network.begin(90, this->Node);
}

void Middleware_OS::Update(){
  network.update();
}

void Middleware_OS::Send(String Message, uint16_t Node){

  Serial.print("Sending: ");
  Serial.println(Message);
  RF24NetworkHeader header(Node);
  bool ok = network.write(header, &Message, sizeof(Message));
}

String Middleware_OS::Receive(){
  String Message;

  while (network.available()){
    RF24NetworkHeader header;
    network.read(header, &Message, sizeof(Message));
  }

  return Message;
}