#include <FB_Const.h>
#include <FB_Error.h>
#include <FB_Network.h>
#include <FB_Utils.h>
#include <Firebase.h>
#include <FirebaseFS.h>
#include <Firebase_ESP_Client.h>
#include <WiFiNINA.h>
#include <Arduino_LSM6DS3.h>
#include "addons/Tokenhelper.h"
#include "addons/RTDBHelper.h"

// EDIT THESE VALUES
const String NODE_NAME = "Node2";
const String NODE_ADDR = "02";
#define IMUREADERROR 0.1
#define TESTING_MODE true

// DO NOT EDIT THESE VALUES

#define API_Key "AIzaSyD5M2Zhd_S7WjA2Z8KxesSECRO9vQgfjrM"
#define DatabaseURL "https://ossweb-97b03-default-rtdb.europe-west1.firebasedatabase.app/" 

#define USER_EMAIL "ethanbarrett11@gmail.com"
#define USER_PASSWORD "EBarrett@01"


/*
#define USER_SSID "Galaxy S21 FE 5G195a"
#define USER_PASS "EBarrett123"
*/

#define USER_SSID "GOLETTINGS"
#define USER_PASS "BAYSWATER"


#define FIREBASE_PROJECT_ID "ossweb-97b03"

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig configuration;

bool signupOK = false;

float prevAx, prevAy, prevAz, Ax, Ay, Az;
float prevGx, prevGy, prevGz, Gx, Gy, Gz;
float Delta_Ax, Delta_Ay, Delta_Az;

String Collection = "NodeInfo/";
String DocumentPath = Collection + NODE_NAME;
String LocalIP = "";


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  WiFi.begin(USER_SSID, USER_PASS);

  Serial.print("Connecting to Wi-Fi");

  while (WiFi.status() != WL_CONNECTED){
    Serial.print(".");
  }

  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  LocalIP = WiFi.localIP();

  configuration.api_key = API_Key;

  
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;
  
  
  configuration.database_url = DatabaseURL;
  configuration.token_status_callback = tokenStatusCallback;

  Firebase.reconnectNetwork(true);
  fbdo.setBSSLBufferSize(4096, 1024);
  fbdo.setResponseSize(4096);

  if (Firebase.signUp(&configuration, &auth, "", "")){
    Serial.print("ok");
  }
  else{
    Serial.println(configuration.signer.signupError.message.c_str());
  }

  Firebase.begin(&configuration, &auth);
  Firebase.reconnectWiFi(true);


  // Start the IMU
  if (!IMU.begin()){
    Serial.println("IMU Failed to Initialize");
  }

  Serial.print("Accelerometer sample rate = ");
  Serial.print(IMU.accelerationSampleRate());
  Serial.println("Hz");
  Serial.println();

  Serial.print("Gyroscope sample rate = ");  
  Serial.print(IMU.gyroscopeSampleRate());
  Serial.println("Hz");
  Serial.println();
}

FirebaseJson Content;
bool taskcompleted = false;
bool change = false;

void loop(){
  /*
  if (Firebase.ready() != taskcompleted){
    taskcompleted = true;

    String collectionID = "NodeInfo";

    Serial.println("Listing the documents in a collection");

    if (Firebase.Firestore.listDocuments(&fbdo, FIREBASE_PROJECT_ID, "", collectionID.c_str(), 4, "", "", "", false)){
      Serial.println(fbdo.payload().c_str());
    }
    else {
      Serial.println(fbdo.errorReason());
    }
  }
  */


  // IF in testing mode only run the IMU and reports back the data and the change between the previous and current data
  if (TESTING_MODE){
    // put all of the previous imu data into the prev option before reassigning them
      prevAx = Ax;
      prevAy = Ay;
      prevAz = Az;
  
      prevGx = Gx;
      prevGy = Gy;
      prevGz = Gz;
      
      if (IMU.accelerationAvailable()) {
        IMU.readAcceleration(Ax, Ay, Az);
    
        Serial.println("Accelerometer data: ");
        Serial.print(Ax);
        Serial.print('\t');
        Serial.print(Ay);
        Serial.print('\t');
        Serial.println(Az);
        Serial.println();
      }
    
      if (IMU.gyroscopeAvailable()) {
        IMU.readGyroscope(Gx, Gy, Gz);
        
        Serial.println("Gyroscope data: ");
        Serial.print(Gx);
        Serial.print('\t');
        Serial.print(Gy);
        Serial.print('\t');
        Serial.println(Gz);
        Serial.println();
      }

      Delta_Ax = prevAx - Ax;
      Delta_Ay = prevAy - Ay;
      Delta_Az = prevAz - Az;
      
  
      // if within range then dont change anything otherwise turn change to true
      if ((-IMUREADERROR < prevAx - Ax < IMUREADERROR) && (-IMUREADERROR < prevAy - Ay < IMUREADERROR) && (-IMUREADERROR < prevAz - Az < IMUREADERROR)){
        change = false;
      }
      else{
        change = true;
      }
  }
  else if (!TESTING_MODE){
    if (Firebase.ready()){
      if (!taskcompleted){
        // Collect the desired Document
        taskcompleted = true;
  
        if (Firebase.Firestore.getDocument(&fbdo, FIREBASE_PROJECT_ID, "", Collection.c_str(), NODE_NAME.c_str())){
          Serial.println(fbdo.payload().c_str());
          
          // Update the document
          Serial.println("Updating Document...");
          Content.clear();
          Content.set("fields/NodeActive/stringValue", "true");
          Content.set("fields/NodeAddr/stringValue", NODE_ADDR.c_str());
          Content.set("fields/NodeName/stringValue", NODE_NAME.c_str());
      
          if (Firebase.Firestore.patchDocument(&fbdo, FIREBASE_PROJECT_ID, "", DocumentPath.c_str(), Content.raw(), "NodeActive, NodeAddr, NodeName")){
            Serial.println("Done");
            Serial.println(fbdo.payload().c_str());
          }
          else{
            Serial.println(fbdo.errorReason());
          }
        }
        else{
          // If the desired Document does not exist, then create the Document
          Serial.println(fbdo.errorReason());
  
          Serial.println("Creating Document...");
  
          Content.clear();
          Content.set("fields/NodeActive/stringValue", "true");
          Content.set("fields/NodeAddr/stringValue", NODE_ADDR.c_str());
          Content.set("fields/NodeName/stringValue", NODE_NAME.c_str());
  
          if (Firebase.Firestore.createDocument(&fbdo, FIREBASE_PROJECT_ID, "", DocumentPath.c_str(), Content.raw())){
            Serial.println(fbdo.payload().c_str());
          }
          else{
            Serial.println(fbdo.errorReason());
          }
        }
      }
  
      // put all of the previous imu data into the prev option before reassigning them
      prevAx = Ax;
      prevAy = Ay;
      prevAz = Az;
  
      prevGx = Gx;
      prevGy = Gy;
      prevGz = Gz;
      
      if (IMU.accelerationAvailable()) {
        IMU.readAcceleration(Ax, Ay, Az);
    
        Serial.println("Accelerometer data: ");
        Serial.print(Ax);
        Serial.print('\t');
        Serial.print(Ay);
        Serial.print('\t');
        Serial.println(Az);
        Serial.println();
      }
    
      if (IMU.gyroscopeAvailable()) {
        IMU.readGyroscope(Gx, Gy, Gz);
        
        Serial.println("Gyroscope data: ");
        Serial.print(Gx);
        Serial.print('\t');
        Serial.print(Gy);
        Serial.print('\t');
        Serial.println(Gz);
        Serial.println();
      }
  
      // if within range then dont change anything otherwise turn change to true
      if ((-IMUREADERROR < prevAx - Ax < IMUREADERROR) && (-IMUREADERROR < prevAy - Ay < IMUREADERROR) && (-IMUREADERROR < prevAz - Az < IMUREADERROR)){
        change = false;
      }
      else{
        change = true;
      }
  
  
      // Update the Values Field with Change if detected
      if (change){
        Content.clear();
        Content.set("fields/NodeAlert/stringValue", "ACTIVE");
        
        if (Firebase.Firestore.patchDocument(&fbdo, FIREBASE_PROJECT_ID, "", DocumentPath.c_str(), Content.raw(), "NodeActive, NodeAddr, NodeName")){
          Serial.println("Done");
          Serial.println(fbdo.payload().c_str());
        }
        else{
          Serial.println(fbdo.errorReason());
        }
      }
    } 
  }
}
