#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>

#define NAMEIMU "god"

Adafruit_BNO055 sensor = Adafruit_BNO055(55,BNO055_ADDRESS_B);

bool IMUInitialized = false;
bool IMUCalibrated = false;
bool IMUPresent = false;
bool Alert = false;
bool enableAlerts = false;
unsigned long CaptureDelay = 100;
imu::Quaternion prevQuat[10];
imu::Quaternion avgQuat;
imu::Vector<3> prevAccel[10];
imu::Vector<3> avgAccel;
int arrayPosition = 0;
int cycles = 0;


void SetupLEDs() {
  pinMode(D7, OUTPUT);
  pinMode(D6, OUTPUT);
}
void StatusAllGood() {
  digitalWrite(D7, HIGH);
  digitalWrite(D6, LOW);
}
void StatusAllBad() {
  digitalWrite(D7, LOW);
  digitalWrite(D6, HIGH);
}

bool displayCalStatus() {
  /* Get the four calibration values (0..3) */
  /* Any sensor data reporting 0 should be ignored, */
  /* 3 means 'fully calibrated" */
  uint8_t system, gyro, accel, mag;
  system = gyro = accel = mag = 0;
  bool sensorExists = true;
  bool calibrated;
  Serial.print("~~");
  sensor.getCalibration(&system, &gyro, &accel, &mag);
  if (sensorExists) {
    /* The data should be ignored until the system calibration is > 0 */
    //Serial.print("\t");
    if (!system) {
      Serial.print("~~");
      Serial.print("NOCAL");
      calibrated = false;
    } else {
      calibrated = true;
    }

    /* Display the individual values */
    //    Serial.print(system, DEC);
    //    Serial.print("G:");
    //    Serial.print(gyro, DEC);
    //    Serial.print("A:");
    //    Serial.print(accel, DEC);
    //    Serial.print("M:");
    //    Serial.print(mag, DEC);

  } else {
    calibrated = true;
    ;
  }
  return calibrated;
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

bool checkMovement(imu::Quaternion quat, imu::Vector<3> accel){
  if (quat.w() > (avgQuat.w() + 0.003) || quat.w() < (avgQuat.w() - 0.003)) {
    return true;
  } else if (accel.x() > (avgAccel.x() + 1) || accel.x() < (avgAccel.x() - 1)){
    return true;
  } if (accel.y() > (avgAccel.y() + 1) || accel.y() < (avgAccel.y() - 1)) {
    return true;
  } if (accel.z() > (avgAccel.z() + 1) || accel.z() < (avgAccel.z() - 1)) {
    return true;
  } else{
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
      if (checkMovement(quat, accel)) {
        Alert = true;
        Serial.println("Alert: Movement detected");
        StatusAllBad();
      } else{
        StatusAllGood();
      }
      prevQuat[arrayPosition] = quat;
      prevAccel[arrayPosition] = accel;
      getAverage();
      arrayPosition++;
      if(arrayPosition >= 10)
        arrayPosition = 0;
      String deets = printQuat(quat, NAMEIMU);
      String speed = printAccel(accel);
      Serial.println(deets + speed);
  }
}

void getAverage()
{
  double w = 0;
  double x = 0;
  double y = 0;
  double z = 0;
  for(int i = 0; i < 10; i++){
    w += prevQuat[i].w();
    x += prevQuat[i].x();
    y += prevQuat[i].y();
    z += prevQuat[i].z();
  }
  avgQuat.w() = w/10;
  avgQuat.x() = x/10;
  avgQuat.y() = y/10;
  avgQuat.z() = z/10;
  x = 0;
  y = 0;
  z = 0;
  for(int i = 0; i < 10; i++){
    x += prevAccel[i].x();
    y += prevAccel[i].y();
    z += prevAccel[i].z();
  }
  avgAccel.x() = x/10;
  avgAccel.y() = y/10;
  avgAccel.z() = z/10;
  
  String averageQuat = printQuat(avgQuat,NAMEIMU);
  String averageAccel = printAccel(avgAccel);
  Serial.println("Average Quat is " + averageQuat + " Average " + averageAccel);
}

void setup() {
  Serial.begin(115200);
  Wire.begin();
  /*Some processors also support 10000 (low speed mode), 1000000 (fast mode plus) and 3400000 (high speed mode). */
  Wire.setClock(400000);  //fastmode.... 100000 is norma
  SetupLEDs();
  enumerateI2CBus();
  InitIMU();
  for(int i = 0; i < 10; i++){
    prevQuat[i] = sensor.getQuat();
    prevAccel[i] = sensor.getVector(Adafruit_BNO055::VECTOR_ACCELEROMETER);
  }
  getAverage();
}

void loop() {
  while (Serial.available() == 0) {
    ReadLineOfData();
    if(cycles > 10 && enableAlerts == false)
    {
      enableAlerts = true;
      if(avgQuat.w() == 0 && avgAccel.x() == 0){
        Serial.println("IMU unresponsive");
        ESP.restart();
      }
    } else {
      cycles++;
    }
    delay(CaptureDelay);
  }
}
