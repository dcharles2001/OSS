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

#define   MESH_PREFIX     "OSS"
#define   MESH_PASSWORD   "FuckROCO"
#define   MESH_PORT       5555
#define   NODE_NAME       "Node1"

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
  Serial.swap();
  if (Serial.available() > 0){
    while (Serial.available() > 0){
      char character = Serial.read();

      Message += String(character);
    }
  }

  Serial.swap();
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
  Serial.println(Message);

  //  PreviousMsg = Message; //payload.Message_to_Send;
  //}
}

void sendMessage() {
  /*
   * @Param: None
   * @Returns: None
   * @Details: Used to broadcast a message
   */
  if (firsttimelaunching){
      String Message = readSerial();//NodeInformation(); // Collect Node Data
      MessageSendingFunction(Message); // Send the Node Data

      firsttimelaunching = false;
  }
  else if (ismaster){

  }

  
  taskSendMessage.setInterval(TASK_SECOND * 10 /* 10 Seconds */ /*random( TASK_SECOND * 1, TASK_SECOND * 5 )*/);
}


// Needed for painless library
void receivedCallback( uint32_t from, String &msg ) {
  Serial.printf("Received from %u msg=%s\n", from, msg.c_str());
  payload.Incoming_Message = msg;
}

void newConnectionCallback(uint32_t nodeId) {
    Serial.printf("New Connection, nodeId = %u\n", nodeId);
}

void changedConnectionCallback() {
  Serial.printf("Changed connections\n");
}

void nodeTimeAdjustedCallback(int32_t offset) {
    Serial.printf("Adjusted time %u. Offset = %d\n", mesh.getNodeTime(),offset);
}

///////////////////////////////////////////////////////////////////////////////////////
// COPY THIS SECTION - COPY THIS SECTION - COPY THIS SECTION - COPY THIS SECTION - CO//
///////////////////////////////////////////////////////////////////////////////////////


void setup() {
  Serial.begin(115200);

//mesh.setDebugMsgTypes( ERROR | MESH_STATUS | CONNECTION | SYNC | COMMUNICATION | GENERAL | MSG_TYPES | REMOTE ); // all types on
  mesh.setDebugMsgTypes( ERROR | STARTUP );  // set before init() so that you can see startup messages

  mesh.init( MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT );
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
