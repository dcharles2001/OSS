#include <ESP8266WiFi.h>
#include <Firebase_ESP_Client.h>
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"

#define WIFI_SSID "GOLETTINGS"
#define WIFI_PASS "BAYSWATER"

#define API_KEY "AIzaSyD5M2Zhd_S7WjA2Z8KxesSECRO9vQgfjrM"
#define RTDB_URL "https://ossweb-97b03-default-rtdb.europe-west1.firebasedatabase.app/"

#define USER_EMAIL "ethanbarrett11@gmail.com"
#define USER_PASSWORD "EBarrett@01"

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  Serial.print("Connecting to WiFi");
  while(WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(300);
  }
  
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());

  config.api_key = API_KEY;
  config.database_url = RTDB_URL;

  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;

  Firebase.reconnectNetwork(true);
  fbdo.setBSSLBufferSize(4096, 1024);
  fbdo.setResponseSize(4096);

  if (Firebase.signUp(&config, &auth, "", "")){
    Serial.println("Ok");
  }
  else{
    Serial.println("%s", config.signer.signupError.message.c_str());
  }

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
}

void loop() {
  // put your main code here, to run repeatedly:
  if (Firebase.ready()){
    // Log Ip address
    Firebase.set(&fbdo, "/NodesIP/IPAddresses", WiFi.localIP().toString().c_str());
    // 
  }
}
