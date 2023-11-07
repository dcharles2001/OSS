#include "Master.h"

RF24 radio(7, 8);
RFNetwork network(radio);

bool NodeSystem::JoinNetwork(){
  if (!this->MemberofNetwork){
    //======JOIN THE NETWORK======//
    SPI.begin();
    radio.begin();
    network.begin(90, this->this_node);

    return true;
  }
}

void NodeSystem::Voting(){
  RF24NetworkHeader header(this->MasterNode);
  String Value = String(this->VotingValue);
  char Message[16] = "VOTE;" +  Value.c_str() + ";";
  
  network.write(header, &Message, sizeof(Message));
}

void NodeSystem::N_TO_M_Update(char *Update){
  RF24NetworkHeader header(this->SecondaryNode);
  network.write(header, )
}