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

#define API_Key "AIzaSyD5M2Zhd_S7WjA2Z8KxesSECRO9vQgfjrM"
#define DatabaseURL "https://ossweb-97b03-default-rtdb.europe-west1.firebasedatabase.app/" 

#define USER_EMAIL "ethanbarrett11@gmail.com"
#define USER_PASSWORD "ethanbarrett02"


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
bool taskcompleted = false;

void loop(){
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
}
