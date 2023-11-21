/*
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/esp-mesh-esp32-esp8266-painlessmesh/
  
  This is a simple example that uses the painlessMesh library: https://github.com/gmag11/painlessMesh/blob/master/examples/basic/basic.ino
*/

///////////////////////////////////////////////////////////////////////////////////////
// COPY THIS SECTION - COPY THIS SECTION - COPY THIS SECTION - COPY THIS SECTION - CO//
///////////////////////////////////////////////////////////////////////////////////////

#include "painlessMesh.h"
#include "Arduino_JSON.h"
//#include <SoftwareSerial.h>
#include <NewPingESP8266.h>

//SoftwareSerial ESPSerial(D7, D8);

#define   MESH_PREFIX     "OSS"
#define   MESH_PASSWORD   "FuckROCO"
#define   MESH_PORT       5555
#define   NODE_NAME       "Node2"

#define TRIGGER_PIN 12 //D6
#define ECHO_PIN 14  //D5
#define sensorPin 13 //D7
#define MAX_DISTANCE 400
#define OPEN_DISTANCE 30

bool firsttimelaunching = true; // dont touch
bool ismaster           = true; // change me please
bool issecondincommand  = false; // dont touch


float Dist;
int Sound;
int Alarm = 0;
int sensorValue;
int sens =0;
NewPingESP8266 sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);

Scheduler userScheduler; // to control your personal task
painlessMesh  mesh;

// User stub
void sendMessage() ; // Prototype so PlatformIO doesn't complain
void sensors();

Task taskSendMessage( TASK_SECOND * 1 , TASK_FOREVER, &sendMessage );
Task sensorMessage( TASK_SECOND * 1 , TASK_FOREVER, &sensors );

struct {
  String Message_to_Send;
  String Incoming_Message;
} payload;

void Voting(){
  /*
   * @Param: None
   * @Returns: None
   * @Details: Used to generate a random voting number
   */
  int NodeNum = sizeof(mesh.getNodeList());
  int vote = random(0, NodeNum);
  String ProtoMessage = "Vote;" + vote;
  String Message = ProtoMessage + ";";

  payload.Message_to_Send = Message;
}
/*
String readSerial(){
  /*
   * @Param: None
   * @Returns: String Message
   * @Details: Used to read the serial Input on D7 and D8
   
  //return "OK";
  String Message;
  //Serial.swap();
  Message = ESPSerial.readStringUntil('\n');

  //Serial.swap();
  return Message;
}
*/
String NodeInformation(){
  JSONVar NodeData;
  NodeData["NodeActive"] = "true";
  NodeData["NodeAddr"]   = String(mesh.getNodeId());
  NodeData["NodeAlarm"]  = "false";
  NodeData["NodeName"]   = NODE_NAME;
  NodeData["Master"]     = true;
  return JSON.stringify(NodeData);
}

void MessageSendingFunction(String Message){
  //Serial.println("Sending String");
  //Serial.println(Message);
  //static String PreviousMsg = "";

  //if (Message != PreviousMsg){
    //String msg = payload.Message_to_Send;
    //msg += mesh.getNodeId();
  mesh.sendBroadcast(Message);
  //Serial.println(Message);

  //  PreviousMsg = Message; //payload.Message_to_Send;
  //}
}

void sendMessage() {
  /*
   * @Param: None
   * @Returns: None
   * @Details: Used to broadcast a message
   */
  //if (firsttimelaunching){
      String Message = "ALL GOOD";//NodeInformation(); // Collect Node Data
      //String Message = readSerial();
      MessageSendingFunction(Message); // Send the Node Data

  //    firsttimelaunching = false;
  //}
  //else if (ismaster){

  //}

  
  taskSendMessage.setInterval(TASK_SECOND * 10 /* 10 Seconds */ /*random( TASK_SECOND * 1, TASK_SECOND * 5 )*/);
}

void sensors(){

  Dist = ((sonar.ping() / 2) * 0.0343);
  sensorValue = digitalRead(sensorPin);
  if (Dist > OPEN_DISTANCE){
    Alarm = 1;
    Serial.println("Safe Open");
    MessageSendingFunction("Safe Open");
    }else{
        Serial.print("Distance = ");
        Serial.print(Dist);
        Serial.println(" cm");
      }
  if (sensorValue == 1){
    sens = 1;
  }
  if (sens == 1){
    Serial.print("Noise Detected");
    MessageSendingFunction("Noise Detected");
    Alarm = 1;
    sens =0;
    }
  
  taskSendMessage.setInterval(TASK_SECOND * 1 /* 10 Seconds */ /*random( TASK_SECOND * 1, TASK_SECOND * 5 )*/);
}

// Needed for painless library
void receivedCallback( uint32_t from, String &msg ) {
  Serial.println(msg.c_str());
  payload.Incoming_Message = msg;
}

void newConnectionCallback(uint32_t nodeId) {
    Serial.println(nodeId);
}

void changedConnectionCallback() {
  Serial.println("Changed connections");
}

void nodeTimeAdjustedCallback(int32_t offset) {
    Serial.println(mesh.getNodeTime());
}

///////////////////////////////////////////////////////////////////////////////////////
// COPY THIS SECTION - COPY THIS SECTION - COPY THIS SECTION - COPY THIS SECTION - CO//
///////////////////////////////////////////////////////////////////////////////////////


void setup() {
  Serial.begin(115200);
//  ESPSerial.begin(9600);
  
  pinMode(sensorPin, INPUT);
  
//mesh.setDebugMsgTypes( ERROR | MESH_STATUS | CONNECTION | SYNC | COMMUNICATION | GENERAL | MSG_TYPES | REMOTE ); // all types on
  mesh.setDebugMsgTypes( ERROR | STARTUP );  // set before init() so that you can see startup messages

  mesh.init(MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT);
  mesh.onReceive(&receivedCallback);
  mesh.onNewConnection(&newConnectionCallback);
  mesh.onChangedConnections(&changedConnectionCallback);
  mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);

  userScheduler.addTask(taskSendMessage);
  taskSendMessage.enable();
  userScheduler.addTask(sensorMessage);
  sensorMessage.enable();
}

void loop() {
  // it will run the user scheduler as well
  //sendMessage("Sorry Dan :)");
  mesh.update();
}
