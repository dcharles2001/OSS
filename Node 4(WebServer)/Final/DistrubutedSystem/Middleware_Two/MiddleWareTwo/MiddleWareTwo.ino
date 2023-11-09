#include "Master.h"

NodeSystem Nodes;

void setup() {
  Serial.begin(9600);
  Nodes.JoinNetwork();
}

void loop() {
  Nodes.RUNSYS();
}
