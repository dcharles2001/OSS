#include "Firebase_Control.h"

const String NODE_NAME = "Node1";
const String NODE_ADDR = "01";

#define API_Key "AIzaSyD5M2Zhd_S7WjA2Z8KxesSECRO9vQgfjrM"
#define DatabaseURL "https://ossweb-97b03-default-rtdb.europe-west1.firebasedatabase.app/" 

#define USER_EMAIL "ethanbarrett11@gmail.com"
#define USER_PASSWORD "EBarrett@01"


#define USER_SSID "Galaxy S21 FE 5G195a"
#define USER_PASS "EBarrett123"


/*
#define USER_SSID "GOLETTINGS"
#define USER_PASS "BAYSWATER"
*/

#define FIREBASE_PROJECT_ID "ossweb-97b03"

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig configuration;

void UpdateDocument(){
  if (Firebase.ready()){
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
    }
  }
}
