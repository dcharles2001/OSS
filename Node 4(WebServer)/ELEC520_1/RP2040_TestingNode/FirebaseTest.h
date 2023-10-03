#ifndef FirebaseTest_H
#define FirebaseTest_H

#include <FB_Const.h>
#include <FB_Error.h>
#include <FB_Network.h>
#include <FB_Utils.h>
#include <Firebase.h>
#include <FirebaseFS.h>
#include <Firebase_ESP_Client.h>
#include "addons/Tokenhelper.h"
#include "addons/RTDBHelper.h"

// Defines
/* EDITABLE Defines */
#define Node_Name "Node1"
#define Node_Addr "01"

/* NON-EDITABLE Defines */
const String NODE_NAME = Node_Name;
const String NODE_ADDR = Node_Addr;

#define API_Key "AIzaSyD5M2Zhd_S7WjA2Z8KxesSECRO9vQgfjrM"
#define DatabaseURL "https://ossweb-97b03-default-rtdb.europe-west1.firebasedatabase.app/" 

#define USER_EMAIL "ethanbarrett11@gmail.com"
#define USER_PASSWORD "EBarrett@01"

#define FIREBASE_PROJECT_ID "ossweb-97b03"

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig configuration;
FirebaseJson Content;
bool taskcompleted = false;
bool change = false;
int NodeNumber;

class FirebaseSystem{
  private:
    String Collection = "NodeInfo/";
    String DocumentPath = Collection + NODE_NAME;
  public:
    void StartFirebase();
    void GetPatchDocument();
    void UpdateDocument();
};

#endif
