/*
 * Author: Ethan Barrett
 * 
 * Last Update: V1.0.1 02/10/2023
 * 
 * Description:
 * The purpose of this section is to read the state of the control panel
 * and other nodes and report any changes back to the cloud, assuming it
 * hasn't already been sent.
 * 
 */

// Libraries
/* FIREBASE Libraries */
#include <FB_Const.h>
#include <FB_Error.h>
#include <FB_Network.h>
#include <FB_Utils.h>
#include <Firebase.h>
#include <FirebaseFS.h>
#include <Firebase_ESP_Client.h>
#include "addons/Tokenhelper.h"
#include "addons/RTDBHelper.h"

/* WIRELESS Libraries */
#include <ESP8266WiFi.h>

/* Additional Libraries */


// Defines
/* EDITABLE Defines */
#define Node_Name "Node1"
#define Node_Addr 01

/* NON-EDITABLE Defines */
const String NODE_NAME = Node_Name;
const String NODE_ADDR = (String)Node_Addr;

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
FirebaseJson Content;
bool taskcompleted = false;
bool change = false;
int NodeNumber;

// Functions and Classes

class FirebaseDocumentation{
  private:
    String Collection = "NodeInfo/";
    String DocumentPath = Collection + NODE_NAME;
  public:
    void StartFirebaseDocumentation(){
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
    
    void GetPatchDocument(){
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
      }
    }

    void UpdateDocument(String Data){
      Content.clear();
      Content.set("fields/NodeAlert/stringValue", Data.c_str());
      
      if (Firebase.Firestore.patchDocument(&fbdo, FIREBASE_PROJECT_ID, "", this->DocumentPath.c_str(), Content.raw(), "NodeActive, NodeAddr, NodeName")){
        Serial.println("Done");
        Serial.println(fbdo.payload().c_str());
      }
      else{
        Serial.println(fbdo.errorReason());
      }
    }

   
};

class System_NRF24L01{
  private:
    // Node addresses in Octal Format
    const uint16_t CurrentNode = Node_Addr;
    
  public:
    
};

// Functions and Class Prototypes
FirebaseDocumentation FD;

void setup() {
  // Connect to the Wi-Fi
  Serial.begin(9600);
  WiFi.begin(USER_SSID, USER_PASS);

  Serial.print("Connecting to Wi-Fi");

  while (WiFi.status() != WL_CONNECTED){
    Serial.print(".");
  }

  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());

  // Start Firebase connection
  FD.StartFirebaseDocumentation();

  // Intitialise the Document
  FD.GetPatchDocument();


  // Find and Return the nodes
  FD.FindNodes();
}

void loop() {
  // put your main code here, to run repeatedly:
  
}
