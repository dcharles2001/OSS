#include "Middleware_OS.h"

void  Middleware_OS::Setup(){
  SPI.begin();
  radio.begin();
  network.begin(90, this->Node)
}

void Middleware_OS::Send(String Message, uint16_t Node){
  RF24NetworkHeader header(this->Node01);
  bool ok = network.write(header, &Message, sizeof(Message))
}

String Middleware_OS::Receive(){
  String Message;

  while (network.available()){
    RF24NetworkHeader header;
    network.read(header, &Message, sizeof(Message));
  }

  return Message;
}