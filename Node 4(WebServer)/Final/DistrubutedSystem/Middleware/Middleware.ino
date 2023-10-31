//https://howtomechatronics.com/tutorials/arduino/how-to-build-an-arduino-wireless-network-with-multiple-nrf24l01-modules/

#include <ESP8266WiFi.h>
#include <SPI.h>
#include <RF24Network>
#include <RF24.h>

RF24 radio(7, 8);
RF24Network NodeNetwork(radio);

uint16_t this_node = 00;
uint16_t Master_Node = 00;
uint16_t NearestNeightbour = 00;


struct {
  bool IsfromCurrent = false;
  uint16_t OriginAddr = 01;
  uint16_t DeliveryAddr = 01;
  char Message[] = "Hello World!";
} payload;

struct {
  bool IsfromCurrent = false;
  uint16_t OriginAddr = 01;
  uint16_t DeliveryAddr = 01;
  char Message[] = "Hello World!";
} payload_incoming;

String Nodes[256] = [];
String NodeAddr[256] = [];

String PathtoMaster[100] = [];
bool Connected_to_Mesh = false;

// function prototypes
void Handshake();
void Path_to_Master();
void Pass_Payload_to_Master();
int  Choose_Master();

void setup() {
  // put your setup code here, to run once:
  SPI.begin();
  radio.begin();
  NodeNetwork.begin(90, this_node);
}

void loop() {
  NodeNetwork.update(); // Check for any updates to the network
  
  if (this_node != Master_Node){
    RF24NetworkHeader header(Master_Node);
    bool ok = NodeNetwork.write(header, &payload, sizeof(payload));
  }
  else if (this_node == Master_Node){
    RF24NetworkHeader header;
    network.read(header, &payload_incoming, sizeof(payload_incoming));
  }

}
