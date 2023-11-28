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
#define   OFF_PIN         D2
#define   ON_PIN          D1

Scheduler userScheduler; // to control your personal task
painlessMesh  mesh;

// User stub
void sendMessage() ; // Prototype so PlatformIO doesn't complain

Task taskSendMessage( TASK_SECOND * 1 , TASK_FOREVER, &sendMessage );

String readSerial(){
  /*
   * @Param: None
   * @Returns: String Message
   * @Details: Used to read the serial Input on D7 and D8
   * @Notes: Used in all nodes that require serial interfacing
   */

  String Message;

  Message = ESPSerial.readStringUntil('\n');

  return Message;
}

String NodeInformation(){
  /*
   * @Param: None
   * @Returns: None
   * @Details: Test connection for online cloud system
   * @Notes: DEPRECIATED
   */


  JSONVar NodeData;
  NodeData["NodeActive"] = "true";
  NodeData["NodeAddr"]   = String(mesh.getNodeId());
  NodeData["NodeAlarm"]  = "false";
  NodeData["NodeName"]   = NODE_NAME;
  NodeData["Master"]     = true;
  return JSON.stringify(NodeData);
}

void MessageSendingFunction(String Message){
  /*
   * @Param: Message <string_var>
   * @Returns: None
   * @Details: used to broadcast a message
   * @Notes: For all nodes
   */
  
  mesh.sendBroadcast(Message);
  
}

void ReadTerminal(){
  /*
   * @Param: None
   * @Returns: None
   * @Details: Used to read the terminal
   * @Notes: Test command for the Main Node only
   */


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
  /*
   * @Param: None
   * @Returns: None
   * @Details: Used to turn off the alarm
   * @Notes: Only for the Main Node Only
   */

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
  /*
   * @Param: Data <string_var>
   * @Returns: None
   * @Details: Used to turn on the On the alarm
   * @Notes: Only for Main Node only
   */

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
  /*
   * @Param: None
   * @Returns: Data <string_var>
   * @Details: Used to read a terminal message
   */



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

      //testing
      Serial.println(Message);
      ReadTerminal();


      // MESH
      //String Message = SerialTerminalRead();
      
      
      MessageSendingFunction(Message); // Send the Node Data

  //    firsttimelaunching = false;
  //}
  //else if (ismaster){

  //}

  
  taskSendMessage.setInterval(TASK_SECOND * 1 /* 10 Seconds */ /*random( TASK_SECOND * 1, TASK_SECOND * 5 )*/);
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

// Needed for painless library
void newConnectionCallback(uint32_t nodeId) {
    Serial.println(nodeId);
}

// Needed for painless library
void changedConnectionCallback() {
  Serial.println("Changed connections");
}

// Needed for painless library
void nodeTimeAdjustedCallback(int32_t offset) {
    Serial.println(mesh.getNodeTime());
}

///////////////////////////////////////////////////////////////////////////////////////
// COPY THIS SECTION - COPY THIS SECTION - COPY THIS SECTION - COPY THIS SECTION - CO//
///////////////////////////////////////////////////////////////////////////////////////


void setup() {
  ///////////////////////////////////////////////////////////////////////////////////////
  // COPY THIS SECTION - COPY THIS SECTION - COPY THIS SECTION - COPY THIS SECTION - CO//
  ///////////////////////////////////////////////////////////////////////////////////////
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

  ///////////////////////////////////////////////////////////////////////////////////////
  // COPY THIS SECTION - COPY THIS SECTION - COPY THIS SECTION - COPY THIS SECTION - CO//
  ///////////////////////////////////////////////////////////////////////////////////////
}

void loop() {
  // it will run the user scheduler as well
  //sendMessage("Sorry Dan :)");
  mesh.update();
}
