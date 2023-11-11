#ifndef Middleware_OS_H
#define Middleware_OS_H

#include <Arduino.h>
#include <RF24.h>
#include <RF24Network.h>
#include <SPI.h>

class Middleware_OS{
  private:
    uint16_t Node = 00; // current node
    uint16_t Node01 = 01;

  public:
    void Setup();
    void Update();
    void Send(String Message, uint16_t Node);
    String Receive();
};

#endif