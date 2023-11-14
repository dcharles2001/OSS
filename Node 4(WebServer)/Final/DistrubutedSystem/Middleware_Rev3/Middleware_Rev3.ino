#include "Middleware_OS.h"

//Middleware_OS MOS;

extern painlessMesh mesh;
extern Scheduler userSchedular;

void setup(){
  Setup();
}

void loop(){
  mesh.update();
}