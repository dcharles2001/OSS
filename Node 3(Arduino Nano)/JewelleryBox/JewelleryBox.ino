#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>

#define NAMEIMU "god"

Adafruit_BNO055 sensor = Adafruit_BNO055(-1, BNO055_ADDRESS_B, 6, -1);

bool QuatMode;
int TakeNumber = 1;
bool IMUInitialized = false;
bool IMUCalibrated = false;
bool IMUPresent = false;
bool Alert = false;
unsigned long CaptureDelay = 100;
const int ledGreenPin = 7;
const int ledRedPin = 6;
imu::Quaternion prevQuat;
imu::Vector<3> prevEuler;


void SetupLEDs() {
  pinMode(ledGreenPin, OUTPUT);
  pinMode(ledRedPin, OUTPUT);
}
void StatusAllGood() {
  digitalWrite(ledGreenPin, HIGH);
  digitalWrite(ledRedPin, LOW);
}
void StatusAllBad() {
  digitalWrite(ledGreenPin, LOW);
  digitalWrite(ledRedPin, HIGH);
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

String printEuler(imu::Vector<3> &euler, String SensorName) {
  String ReturnString = "~~";
  ReturnString += SensorName;
  // Display the floating point data
  ReturnString += "~X:";
  ReturnString += String(euler.x());
  ReturnString += "~Y:";
  ReturnString += String(euler.y());
  ReturnString += "~Z:";
  ReturnString += String(euler.z());
  ReturnString += "~";

  return ReturnString;
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
    if (QuatMode) {
      // Quaternion data
      imu::Quaternion quat = sensor.getQuat();
      if (quat.w() > (prevQuat.w() + 0.004) || quat.w() < (prevQuat.w() - 0.004) || quat.x() > (prevQuat.x() + 0.01) || quat.x() < (prevQuat.x() - 0.01)) {
        Alert = true;
        Serial.println("Alert: Movement detected");
      }
      prevQuat = quat;
      String deets = printQuat(quat, NAMEIMU);
      Serial.println(deets);
    } else {
      imu::Vector<3> euler = sensor.getVector(Adafruit_BNO055::VECTOR_EULER);
      if (euler.x() > (prevEuler.x() + 0.1) || euler.x() < (prevEuler.x() - 0.1)) {
        Alert = true;
        Serial.println("Alert: Movement detected");
      }
      prevEuler = euler;
      String deets = printEuler(euler, NAMEIMU);
      Serial.println(deets);
    }
  }
}

void setup() {
  Wire.begin();
  /*Some processors also support 10000 (low speed mode), 1000000 (fast mode plus) and 3400000 (high speed mode). */
  Wire.setClock(400000);  //fastmode.... 100000 is normal
  QuatMode = false;
  Serial.begin(115200);
  SetupLEDs();
  enumerateI2CBus();
  InitIMU();
  // if (IMUInitialized) {
  //   while (!displayCalStatus() && (Serial.available() == 0)) {
  //     delay(100);
  //   }
  //   IMUCalibrated = true;
  // } else {
  //   StatusAllBad();
  //   Serial.println("You Must Initialize the Sensors Prior to Calibration");
  //   delay(2000);
  // }
  if (QuatMode) {
    prevQuat = sensor.getQuat();
  }
  else
  {
    prevEuler = sensor.getVector(Adafruit_BNO055::VECTOR_EULER);
  }
}

void loop() {
  while (Serial.available() == 0) {
    ReadLineOfData();
    delay(CaptureDelay);
  }
}
