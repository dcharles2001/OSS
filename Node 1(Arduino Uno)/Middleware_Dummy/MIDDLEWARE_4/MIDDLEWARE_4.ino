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
#include <SoftwareSerial.h>

SoftwareSerial ESPSerial(D7, D8);

#define   MESH_PREFIX     "OSS"
#define   MESH_PASSWORD   "FuckROCO"
#define   MESH_PORT       5555
#define   NODE_NAME       "Node0"
#define   OFF_PIN         D5
#define   ON_PIN          D4

bool firsttimelaunching = true; // dont touch
bool ismaster           = true; // change me please
bool issecondincommand  = false; // dont touch

Scheduler userScheduler; // to control your personal task
painlessMesh  mesh;

// User stub
void sendMessage() ; // Prototype so PlatformIO doesn't complain

Task taskSendMessage( TASK_SECOND * 1 , TASK_FOREVER, &sendMessage );

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

String readSerial(){
  /*
   * @Param: None
   * @Returns: String Message
   * @Details: Used to read the serial Input on D7 and D8
   */
  //return "OK";
  String Message;
  //Serial.swap();
  Message = ESPSerial.readStringUntil('\n');

  //Serial.swap();
  return Message;
}

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

void ReadTerminal(){
  if (digitalRead(OFF_PIN) == HIGH){
    digitalWrite(OFF_PIN, LOW);
  }

  if (Serial.available() > 0){
    String Data= Serial.readStringUntil(',');
    Serial.println("Command Accepted");

    if (Data == "off"){
      Serial.println("Turning Off Alarm");
      digitalWrite(OFF_PIN, HIGH);
    }
  }
}

bool Off = true;

void off(String Data){
  if (digitalRead(OFF_PIN) == HIGH){
    digitalWrite(OFF_PIN, LOW);
  }

  if ((Data == "off") && (Off == false)){
      //Serial.println("Turning Off Alarm");
      digitalWrite(OFF_PIN, HIGH);
      digitalWrite(ON_PIN, LOW);

      Off = true;
  }
}

void On(String Data){
  if (digitalRead(ON_PIN) == HIGH){
    digitalWrite(ON_PIN, LOW);
  }

  if ((Data == "ER01") && (Off == true)){
    //Serial.println("Turning On Alarm");
    digitalWrite(ON_PIN, HIGH);
    digitalWrite(OFF_PIN, LOW);
    Off = false;
  }
}

String SerialTerminalRead(){
  String Data;
  if (Serial.available() > 0){
    Data = Serial.readStringUntil(',');
    Serial.println("Command Accepted");
  }

  return Data;
}

void sendMessage() {
  /*
   * @Param: None
   * @Returns: None
   * @Details: Used to broadcast a message
   */
  //if (firsttimelaunching){
      //String Message = NodeInformation(); // Collect Node Data

      digitalWrite(OFF_PIN, LOW);
      digitalWrite(ON_PIN, LOW);

      // MAIN NODE
      String Message = readSerial();


      // MESH
      //String Message = SerialTerminalRead();
      
      
      MessageSendingFunction(Message); // Send the Node Data

  //    firsttimelaunching = false;
  //}
  //else if (ismaster){

  //}

  
  taskSendMessage.setInterval(TASK_SECOND * 10 /* 10 Seconds */ /*random( TASK_SECOND * 1, TASK_SECOND * 5 )*/);
}


// Needed for painless library
void receivedCallback( uint32_t from, String &msg ) {
  Serial.println(msg.c_str());

  if (msg == "off"){
    off(msg);
  }
  
  if (msg == "ER01"){
    On(msg);
  }
  
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
  ESPSerial.begin(9600);
  pinMode(OFF_PIN, OUTPUT);
  pinMode(ON_PIN, OUTPUT);

  digitalWrite(OFF_PIN, LOW);
  digitalWrite(ON_PIN, LOW);

//mesh.setDebugMsgTypes( ERROR | MESH_STATUS | CONNECTION | SYNC | COMMUNICATION | GENERAL | MSG_TYPES | REMOTE ); // all types on
  mesh.setDebugMsgTypes( ERROR | STARTUP );  // set before init() so that you can see startup messages

  mesh.init(MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT);
  mesh.onReceive(&receivedCallback);
  mesh.onNewConnection(&newConnectionCallback);
  mesh.onChangedConnections(&changedConnectionCallback);
  mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);

  userScheduler.addTask(taskSendMessage);
  taskSendMessage.enable();
}

void loop() {
  // it will run the user scheduler as well
  //sendMessage("Sorry Dan :)");
  mesh.update();
}
