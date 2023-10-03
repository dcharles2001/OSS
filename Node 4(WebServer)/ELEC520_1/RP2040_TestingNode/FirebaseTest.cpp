#include "FirebaseTest.h"

void FirebaseSystem::StartFirebase(){
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

void FirebaseSystem::GetPatchDocument(){
  
}

void FirebaseSystem::UpdateDocument(){
  
}
