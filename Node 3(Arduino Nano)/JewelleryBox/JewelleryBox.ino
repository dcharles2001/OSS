#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>

#include "painlessMesh.h"
#include "Arduino_JSON.h"

#define MESH_PREFIX "OSS"
#define MESH_PASSWORD "OSSDSCAM"
#define MESH_PORT 5555
#define NODE_NAME "Node3"

Scheduler userScheduler;  // to control your personal task
painlessMesh mesh;

// User stub
void sendMessage();  // Prototype so PlatformIO doesn't complain
void ReadLineOfData();

Task taskSendMessage(TASK_SECOND * 1, TASK_FOREVER, &sendMessage);
Task checkIMU(TASK_SECOND * 0.5, TASK_FOREVER, &ReadLineOfData);

struct {
  String Message_to_Send;
  String Incoming_Message;
} payload;

String NodeInformation() {
  /*
   * @Param: None
   * @Returns: None
   * @Details: Test connection for online cloud system
   * @Notes: DEPRECIATED
   */


  JSONVar NodeData;
  NodeData["NodeActive"] = "true";
  NodeData["NodeAddr"] = String(mesh.getNodeId());
  NodeData["NodeAlarm"] = "false";
  NodeData["NodeName"] = NODE_NAME;
  NodeData["Master"] = true;
  return JSON.stringify(NodeData);
}

void MessageSendingFunction(String Message) {
  /*
   * @Param: Message <string_var>
   * @Returns: None
   * @Details: used to broadcast a message
   * @Notes: For all nodes
   */

  mesh.sendBroadcast(Message);
}

void sendMessage() {
  /*
   * @Param: None
   * @Returns: None
   * @Details: Used to broadcast a message
   */
  //if (firsttimelaunching){
      String Message = "Node3 GOOD";//NodeInformation(); // Collect Node Data
      //String Message = readSerial();
      MessageSendingFunction(Message); // Send the Node Data

  //    firsttimelaunching = false;
  //}
  //else if (ismaster){

  //}

  
  taskSendMessage.setInterval(TASK_SECOND * 1 /* 10 Seconds */ /*random( TASK_SECOND * 1, TASK_SECOND * 5 )*/);
}



// Needed for painless library

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

#define NAMEIMU "Jewellery Box"

Adafruit_BNO055 sensor = Adafruit_BNO055(55, BNO055_ADDRESS_B);

bool IMUInitialized = false;
bool IMUCalibrated = false;
bool IMUPresent = false;
bool Alert = true;
bool enableAlerts = false;
unsigned long CaptureDelay = 100;
imu::Quaternion prevQuat[10];
imu::Quaternion avgQuat;
imu::Vector<3> prevAccel[10];
imu::Vector<3> avgAccel;
int arrayPosition = 0;
int cycles = 0;
bool Unlock = false;

void locking(bool Unlock){
  Serial.println("Hello World!");
  if (Unlock){
    Serial.println("System Unlocked");
    taskSendMessage.disable();
    checkIMU.disable();
    userScheduler.deleteTask(taskSendMessage);
    userScheduler.deleteTask(checkIMU);
  }
  else if (!Unlock){
    Serial.println("System Locked");
    userScheduler.addTask(taskSendMessage);
    userScheduler.addTask(checkIMU);
    taskSendMessage.enable();
    checkIMU.enable();
  }

}


void receivedCallback( uint32_t from, String &msg ) {
  Serial.println(Unlock);
  Serial.println(msg.c_str());
  if(msg == "UNLK"){
    Unlock = true;
    Serial.println("World!");
    locking(Unlock);
  }
  if(msg == "LOCK"){
    Unlock = false;
    Serial.println("Hello!");
    locking(Unlock);
  }
}


void SetupLEDs() {
  pinMode(D7, OUTPUT);
  pinMode(D6, OUTPUT);
}
void StatusAllGood() {
  MessageSendingFunction("Node3 GOOD");
  digitalWrite(D7, HIGH);
  digitalWrite(D6, LOW);
}
void StatusAllBad() {
  digitalWrite(D7, LOW);
  digitalWrite(D6, HIGH);
}

void enumerateI2CBus(void) {
  byte error, address;
  int nDevices;

  Serial.println("");
  Serial.println("Scanning...");

  nDevices = 0;
  for (address = 1; address < 127; address++) {
    // The i2c_scanner uses the return value of
    // the Write.endTransmisstion to see if
    // a device did acknowledge to the address.
    Wire.beginTransmission(address);
    error = Wire.endTransmission();

    if (error == 0) {
      Serial.print("I2C device found at address 0x");
      if (address < 16)
        Serial.print("0");
      Serial.print(address, HEX);
      Serial.println("  !");

      nDevices++;
    } else if (error == 4) {
      Serial.print("Unknown error at address 0x");
      if (address < 16)
        Serial.print("0");
      Serial.println(address, HEX);
    }
  }
  if (nDevices == 0)
    Serial.println("No I2C devices found\n");
  else
    Serial.println("done\n");
}

void InitIMU(void) {
  /* Initialise the sensor */
  if (!sensor.begin()) {
    /* There was a problem detecting the BNO055 ... check your connections */
    Serial.println("IMU Not Found.");
    IMUPresent = false;
    StatusAllBad();
  } else {
    Serial.println("IMU Initialized..");
    IMUPresent = true;
    IMUInitialized = true;
    sensor.setExtCrystalUse(true);
    StatusAllGood();
  }
  /* Display the current temperature */
  //int8_t temp = sensor.getTemp();
  //Serial.print("Current Temperature: ");
  //Serial.print(temp);
  //Serial.println(" C");
}

String printQuat(imu::Quaternion &quat, String SensorName) {
  String ReturnString = "~~";
  ReturnString += SensorName;
  ReturnString += "~qW:";
  ReturnString += String(quat.w(), 4);
  ReturnString += "~qX:";
  ReturnString += String(quat.y(), 4);
  ReturnString += "~qY:";
  ReturnString += String(quat.x(), 4);
  ReturnString += "~qZ:";
  ReturnString += String(quat.z(), 4);
  ReturnString += "~";
  return ReturnString;
}

String printAccel(imu::Vector<3> &accel) {
  String ReturnString = "Speed: ";
  ReturnString += "Sideways:";
  ReturnString += String(accel.x(), 4);
  ReturnString += "~Approach:";
  ReturnString += String(accel.y(), 4);
  ReturnString += "~Vertical:";
  ReturnString += String(accel.z(), 4);
  ReturnString += "~";
  return ReturnString;
}

bool checkMovement(imu::Quaternion quat, imu::Vector<3> accel) {
  if (quat.w() > (avgQuat.w() + 0.003) || quat.w() < (avgQuat.w() - 0.003)) {
    return true;
  } else if (accel.x() > (avgAccel.x() + 1) || accel.x() < (avgAccel.x() - 1)) {
    return true;
  }
  if (accel.y() > (avgAccel.y() + 1) || accel.y() < (avgAccel.y() - 1)) {
    return true;
  }
  if (accel.z() > (avgAccel.z() + 1) || accel.z() < (avgAccel.z() - 1)) {
    return true;
  } else {
    return false;
  }
}

void ReadLineOfData() {
  if (IMUPresent) {
    // Possible vector values can be:
    // - VECTOR_ACCELEROMETER - m/s^2
    // - VECTOR_MAGNETOMETER  - uT
    // - VECTOR_GYROSCOPE     - rad/s
    // - VECTOR_EULER         - degrees
    // - VECTOR_LINEARACCEL   - m/s^2
    // - VECTOR_GRAVITY       - m/s^2
    imu::Quaternion quat = sensor.getQuat();
    imu::Vector<3> accel = sensor.getVector(Adafruit_BNO055::VECTOR_ACCELEROMETER);
    if (checkMovement(quat, accel) && enableAlerts == true) {
      Alert = true;
      Serial.println("Alert: Movement detected");
      //MessageSendingFunction("Alert: Movement detected");
      MessageSendingFunction("ER01");
      StatusAllBad();
    } else if(checkMovement(quat,accel) == false){
      //MessageSendingFunction("Node3 GOOD");
      Alert = false;
      StatusAllGood();
    }
    prevQuat[arrayPosition] = quat;
    prevAccel[arrayPosition] = accel;
    getAverage();
    arrayPosition++;
    if (arrayPosition >= 10)
      arrayPosition = 0;
    String deets = printQuat(quat, NAMEIMU);
    String speed = printAccel(accel);
    //Serial.println(deets + speed);
    delay(CaptureDelay);
    taskSendMessage.setInterval(TASK_SECOND * 1 /* 10 Seconds */ /*random( TASK_SECOND * 1, TASK_SECOND * 5 )*/);
  }
}

void getAverage() {
  double w = 0;
  double x = 0;
  double y = 0;
  double z = 0;
  for (int i = 0; i < 10; i++) {
    w += prevQuat[i].w();
    x += prevQuat[i].x();
    y += prevQuat[i].y();
    z += prevQuat[i].z();
  }
  avgQuat.w() = w / 10;
  avgQuat.x() = x / 10;
  avgQuat.y() = y / 10;
  avgQuat.z() = z / 10;
  x = 0;
  y = 0;
  z = 0;
  for (int i = 0; i < 10; i++) {
    x += prevAccel[i].x();
    y += prevAccel[i].y();
    z += prevAccel[i].z();
  }
  avgAccel.x() = x / 10;
  avgAccel.y() = y / 10;
  avgAccel.z() = z / 10;

 // String averageQuat = printQuat(avgQuat, NAMEIMU);
  //String averageAccel = printAccel(avgAccel);
  //Serial.println("Average Quat is " + averageQuat + " Average " + averageAccel);
}

void setup() {
  Serial.begin(115200);
  Wire.begin();
  /*Some processors also support 10000 (low speed mode), 1000000 (fast mode plus) and 3400000 (high speed mode). */
  Wire.setClock(400000);  //fastmode.... 100000 is normal
  SetupLEDs();
  enumerateI2CBus();
  InitIMU();
  while(Alert == true)
  {
    ReadLineOfData();
  }
  if (avgQuat.w() == 0 && avgAccel.x() == 0) {
    Serial.println("IMU unresponsive");
    ESP.restart();
  }
  enableAlerts = true;

  mesh.setDebugMsgTypes(ERROR | STARTUP);  // set before init() so that you can see startup messages

  mesh.init(MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT);
  mesh.onReceive(&receivedCallback);
  mesh.onNewConnection(&newConnectionCallback);
  mesh.onChangedConnections(&changedConnectionCallback);
  mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);

  userScheduler.addTask(taskSendMessage);
  taskSendMessage.enable();
  userScheduler.addTask(checkIMU);
  checkIMU.enable();
}

void loop() {
  mesh.update();
}
