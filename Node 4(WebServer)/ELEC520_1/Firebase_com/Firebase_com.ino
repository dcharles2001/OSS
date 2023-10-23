#include <FB_Const.h>
#include <FB_Error.h>
#include <FB_Network.h>
#include <FB_Utils.h>
#include <Firebase.h>
#include <FirebaseFS.h>
#include <Firebase_ESP_Client.h>
#include <ESP8266WiFi.h>
#include "addons/Tokenhelper.h"
#include "addons/RTDBHelper.h"
#include <ESP32Ping.h>

// EDIT THESE VALUES
const String NODE_NAME = "Node1";
const String NODE_ADDR = "01";

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

String WifiIP = "";

// Gossip Algorithm

struct {
  bool alive = true;
  bool isalive = false;
  bool firebasealive = true;
  char* Ipaddress = "";
  char* RecvIpaddress = "";
  char* nameofrecv = "";
  bool alarm = false;
} Gossip_Package;

IPAddress ip;

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
  WifiIP = WiFi.localIP().toString();
  ip = WiFi.localIP();

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
}

FirebaseJson Content;
FirebaseJsonData Result;
bool taskcompleted = false;
bool change = false;
bool RESET = false;
char incomingByte = 'A';
String Ipaddresses[255];

// Function Prototypes
void rtdbDownloadCallback(RTDB_DownloadStatusInfo info);
void Gossip();

void loop(){
  
  // Check if the Firebase has a document and if so update it and if not create one and update it.
  if (Firebase.ready()){
    Gossip_Package.firebasealive = true;
    String Collection = "NodeInfo/";
    String DocumentPath = Collection + NODE_NAME;

    if (!taskcompleted){
      // Collect the desired Document
      taskcompleted = true;

      if (Firebase.Firestore.getDocument(&fbdo, FIREBASE_PROJECT_ID, "", Collection.c_str(), NODE_NAME.c_str())){
        Serial.println(fbdo.payload().c_str());
        
        // Update the document
        Serial.println("Updating Document...");
        Content.clear();
        Content.set("fields/NodeActive/stringValue", "true");
        Content.set("fields/NodeAddr/stringValue", WifiIP.c_str());
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

      Serial.println("Getting Indexes");
    }
    
    /*
    // set up the system to receive commands and then display them on the system
    if ((Serial.available() > 0) && (RESET == false)){
      incomingByte = Serial.read();
      //RESET = true;
    }
    */
  }

  // Check firebase
  else if (!firebase.ready()){
    Gossip_Package.firebasealive = false;
  }

  // Check for Ip addresses


  if (incomingByte == 'A'){
    // Firmware OTA Update
    Serial.println("Starting Firmware Update!");

    if(!Firebase.RTDB.downloadOTA(&fbdo, F("firmware/bin"), rtdbDownloadCallback)){
      Serial.println(fbdo.errorReason());
    }
  }
  else if  (incomingByte == 'B'){
    // other application
  }
}

void rtdbDownloadCallback(RTDB_DownloadStatusInfo info){
  if(info.status == firebase_rtdb_download_status_init){
    Serial.print("Downloading Firmware File");
    Serial.print(info.remotePath.c_str());
    Serial.println(info.size);
  }
  else if (info.status == firebase_rtdb_download_status_download){
    Serial.print("Downloaded: ");
    Serial.print((int)info.progress);
    Serial.print("%, Elapsed Time: ");
    Serial.print(info.elaspedTime);
    Serial.println("ms");
  }
  else if (info.status == firebase_rtdb_download_status_complete){
    Serial.println("Update Firmware completed!");
    Serial.println("Restarting...");
    delay(200);
    ESP.restart();
  }
  else if (info.status == firebase_rtdb_download_status_error){
    Serial.println("Download Failed");
    Serial.println(info.errorMsg.c_str());
  }
}

WifiClient client;

void Gossip(){
  String ipbody = "";
  String ipbody_string;

  ipbody_string = ipbody + ip[0] + "." + ip[1] + "." + ip[2] + ".";
  const char *remoteHost;


  // check for ip addresses
  int noips = 0;
  for (int i = 0; i < 255; i++){
    ipbody_string = ipbody + i;
    remoteHost = ipbody_string.c_str();

    if (Ping.ping(remoteHost)){
      Ipaddresses[noips];
      // send a message
      // NodeName, Ipaddress, active, alarm
      String Package = NODE_NAME + "," + ip + "," + Gossip_Package.active + "," + Gossip_Package.alarm;
      connect(remoteHost, 80);

      client.print(Package.c_str());

      Serial.println("Sending Package...");
      Serial.println(Package.c_str());

      noips++;
    }
  }

  if (Gossip_Package.firebasealive){
    // update firebase of alarm
    if (Gossip_Package.alarm){
      Content.clear();
      Content.set("fields/NodeAlarm/stringValue", "true");

      // pacth document for the app to recognise
      Firebase.Firestore.patchDocument(&fbdo, FIREBASE_PROJECT_ID, "", "NodeInfo/Node1" , Content.raw());
    }
  }
}