#include "Master.h"

NodeSystem Nodes;

void setup() {
  Serial.begin(9600);
  delay(100);

  Nodes.JoinNetwork();
}

void loop() {
  Nodes.UpdateNetwork();
  //Nodes.RUNSYS();
  //String M = Nodes.Receive();
  //Serial.println(M);

  Nodes.PingMaster();
}