//https://howtomechatronics.com/tutorials/arduino/how-to-build-an-arduino-wireless-network-with-multiple-nrf24l01-modules/

#include <ESP8266WiFi.h>
#include <SPI.h>
#include <RF24Network.h>
#include <RF24.h>

RF24 radio(7, 8);
RF24Network network(radio);

uint16_t this_node = 00;
uint16_t Master_Node = 00;
uint16_t Secondary_Node == 00;

uint16_t PrimeNumber = 7;

struct {
  char Message[32] = "";
} payload;

// MASTER AND SECOND IN COMMAND INFO ONLY
int Nodelist[32];
int NodePosition = 1;

void setup() {
  // put your setup code here, to run once:
  SPI.begin();
  radio.begin();
  network.begin(90, this_node);

  if ((this_node == Master_Node) || (this_node == Secondary_Node){
    NodeList[0] = Master_Node;
  }
}

char MODE[8] = ""; 
/*
Mode Options
UPLD -> Upload Data to cloud
EROR -> Error Alert
PING -> Check System is Active
VOTE -> Voting for 2nd in command
VTST -> Start the Voting
GMSG -> Global Message
ATTN -> Attach Node to system
*/

void loop() {
  network.update(); // Check for any updates to the network
  char MessageData[32] = ""; // reset MessageData
  char *token;

  // clear memory for lists
  memset(NodeList, 0, sizeof(NodeList));
  
  if (this_node == Master_Node){
    //======RECEIVING INFORMATION======//
    while (network.available()){
      RF24NetworkHeader Header;
      network.read(Header, &MessageData, sizeof(MessageData));
    }
    // run commands (split with a ,)

    token = strtok(MessageData, ",");

    if (token != NULL){
      if (token == "UPLD"){
        // Upload Data to Firestore
        Serial.println("Uploading Data to firestore");
        token = strtok(NULL, ",");
        Serial.println(token);
      }
      else if (token == "EROR"){
        // Report Error
        Serial.println("Error has occured!")
        token = strtok(NULL,  ",");
        Serial.println(token);
      }
      else if (token == "PING"){
        // Respond to ping
        payload.message = "OK";

        RF24NetworkHeader header(Secondary_Node);
        network.write(header, &payload.message, sizeof(payload.message));

      }
      else if (token == "VOTE"){
        // Voting system
        // multiply them
        token = strtok(NULL, ",");
        int value = atoi(token);
        
        // Talk to Tom!
        // why can we count how many of each value there are in a list instead of using prime numbers?

      }
      else if (token == "GSMG"){
        // displays the general message
        token = strtok(NULL, ",");
        Serial.println(token);
      }
      else if (token == "ATTN"){
        token = strtok(NULL, ",");

        Serial.print("Adding Node: ")
        Serial.print(token);
        Serial.println(" to Network")

        int NodeAddr = atoi(token);

        NodeList[NodePosition] = NodeAddr;
        NodePosition++;
      }
    }
  }

  else if (this_node == Secondary_Node){
    //======COMMUNICATES WITH THE MASTER NODE======//

    //======CHECK MASTERNODE IS ONLINE======//
    payload.message = "PING";

    RF24NetworkHeader header(Secondary_Node);
    network.write(header, &payload.message, sizeof(payload.message));

    //======WAIT FOR RESPONSE======//
    while (network.available()){
      RF24NetworkHeader Header;
      network.read(Header, &MessageData, sizeof(MessageData));
    }

    if (MessageData == "OK"){
      Serial.println("Master Okay");
    }
    else {
      Serial.println("Master Offline, Second in command now master, Starting voting system");

      // Secondary Node becomes Master
      Master_Node = Secondary_Node;

      for (int iterator = 1; iterator < sizeof(NodeList); iterator++){
        int Destination = NodeList[iterator];
        RF24NetworkHeader header(Destination);
        char Message[32] = "VTST";
        network.write(header, &Message, sizeof(Messgae));
      }
    }
  }

  else if ((this_node != Master_Node) && (this_node != Secondary_Node)){
    //======CHECK FOR VOTING INPUT======//
    while (network.available()){
      RF24NetworkHeader Header;
      network.read(Header, &MessageData, sizeof(MessageData));
    }

    if (MessageData == "VTST"){
      Serial.println("Starting Voting");
      Master_Node = Secondary_Node;

      payload.message = "VOTE," + str(PrimeNumber) + ",";

      RF24NetworkHeader header(Master_Node);
      network.write(header, &payload.message, sizeof(payload.message));

    }

    //======TRANSMITTING INFORMATION======//
    
    //======TRANSMIT======//
    RF24NetworkHeader header(Secondary_Node);
    network.write(header, &payload.message, sizeof(payload.message));
  }
}
