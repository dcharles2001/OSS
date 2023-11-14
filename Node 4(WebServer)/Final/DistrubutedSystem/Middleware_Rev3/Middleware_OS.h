#ifndef Middleware_OS_H
#define Middleware_OS_H

extern painlessMesh mesh;
extern Scheduler userSchedular;

void Setup();
void sendMessage();
void receivedCallback( uint32_t from, String &msg );
void newConnectionCallback(uint32_t nodeId);
void changedConnectionCallback();
void nodeTimeAdjustedCallback(int32_t offset);

#endif