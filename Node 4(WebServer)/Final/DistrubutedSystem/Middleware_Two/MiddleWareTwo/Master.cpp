#include "Master.h"

RF24 radio(D4, D2);
RF24Network network(radio);

///////////////////////////////
//======BASIC FUNCTIONS======//
///////////////////////////////

void NodeSystem::JoinNetwork(){
  //======JOIN THE NETWORK======//
  SPI.begin();
  radio.begin();
  radio.setChannel(90);
  network.begin(this->this_node);

  //======SEND THE ADDRESS OF THE NEW NODE TO THE MASTER======//
  RF24NetworkHeader header(this->MasterNode);
  String ProtoMessage = "ATTN;" + this->this_node;
  String Message = ProtoMessage + ";";

  network.write(header, &Message, sizeof(Message));

}

void NodeSystem::RejoinNetwork(){
  //======SEND THE ADDRESS OF THE NEW NODE TO THE MASTER======//
    RF24NetworkHeader header(this->MasterNode);
    String ProtoMessage = "ATTN;" + this->this_node;
    String Message = ProtoMessage + ";";

    network.write(header, &Message, sizeof(Message));
}

void NodeSystem::Voting(int nodenum){
  RF24NetworkHeader header(this->MasterNode);
  int vote = random(1, nodenum); // random voting value of node
  String ProtoMessage = "VOTE;" +  vote;
  String Message = ProtoMessage  + ";";
  
  network.write(header, &Message, sizeof(Message));
}

void NodeSystem::N_TO_M_Update(String Update){
  //======TO BE COMPLETED======//
}

void NodeSystem::UpdateToSecondinCommand(){
  this->SecondaryNode = this->this_node;
}

String NodeSystem::Receive(){
  String Message;

  while (network.available()){
    RF24NetworkHeader RecvHeader;
    network.read(RecvHeader, &Message, sizeof(Message));
  }

  return Message;
}

///////////////////////////////////////////
//======SECOND IN COMMAND FUNCTIONS======//
///////////////////////////////////////////

void NodeSystem::UpdateToMaster(){
  this->MasterNode = this->this_node;
}

void NodeSystem::PingMaster(){
  //======TRANSMITTING======//
  String Message = "PING;";
  String MessageResponse;
  RF24NetworkHeader header(this->MasterNode);
  network.read(header, &Message, sizeof(Message));

  delay(10);

  while (network.available()){
    RF24NetworkHeader RecvHeader;
    network.read(RecvHeader, &MessageResponse, sizeof(MessageResponse));
  }

  if (MessageResponse == "PONG"){
    Serial.println("Ping Successful");
  }
  else{
    Serial.println("Ping Failed");
    
    //======CONVERT TO MASTER======//
    this->UpdateToMaster();
    this->CommandRejoinNetwork();
  }
}

void NodeSystem::CommandRejoinNetwork(){
  String Message = "REJN;";

  RF24NetworkHeader header;
  network.multicast(header, &Message, sizeof(Message), 1);
}

////////////////////////////////
//======MASTER FUNCTIONS======//
////////////////////////////////

void NodeSystem::M_TO_N_Update(String Update){
  //======TO BE COMPLETED======//
}

void NodeSystem::Upload(String message /*=""*/){
  // Upload to Firebase
  //======TO BE COMPLETED======//
}

void NodeSystem::VoteRecv(String message, int nodenum, bool votestart, bool votecomplete){
  
  if ((votestart) && (!votecomplete)){
    // starting of the voting system
    memset(this->ListOfVotes, 0, sizeof(this->ListOfVotes)); // clear the list
    this->VotingPos = 0; // reset voting position
    
    int vote = random(1, nodenum);

    this->ListOfVotes[0] = vote;

    this->VotingPos++;
  }
  else if ((!votestart) && (!votecomplete)){
    String input;
    //input = strtok(message, ";");
    int idx = message.indexOf(";");
    input = message.substring(0, idx);


    int inputVal;
    inputVal = input.toInt();
    //inputVal = atoi(input);

    this->ListOfVotes[this->VotingPos] = inputVal;

    this->VotingPos++;
  }
  else if (votecomplete){
    // voting complete, tally the results
    for (int votenumber = 1; votenumber < nodenum; votenumber++){
      int count = 0;
      for (int i; i < this->VotingPos; i++){
        if (this->ListOfVotes[i] == votenumber){
          count++;
        }
      }

      this->AmountOfEachVote[votenumber] = count;
    }

    // find the one with the greatest count
    int greatestIndex = 0;
    for (int n = 1; n < sizeof(this->AmountOfEachVote); n++){
      if (this->AmountOfEachVote[greatestIndex] < this->AmountOfEachVote[n]){
        greatestIndex = n;
      }
    }
    Serial.print("The winning node is: node ");
    Serial.print(greatestIndex);
    Serial.print(" , with ");
    Serial.print(this->AmountOfEachVote[greatestIndex]);
    Serial.println(" votes");

    this->SecondaryNode = greatestIndex;

    RF24NetworkHeader header(this->SecondaryNode);
    char Message[8] = "SINC";  // SINC = Second IN Command
    network.write(header, &Message, sizeof(Message)); // send the correction term to the second in command
  }

}

void NodeSystem::AddNode(String message){

  int idx = message.indexOf(";");
  String Message = message.substring(0, idx);

  this->Addresses[this->AddressPos] = Message.toInt();
  this->AddressPos++;  
}

void NodeSystem::RespondToPing(){
  //======TRANSMITTING======//
  String Message = "PONG";
  RF24NetworkHeader header(this->SecondaryNode);
  network.write(header, &Message, sizeof(Message));
}

void NodeSystem::StartVote(){
  /*
  String Message = "VTST;"
  for (int i = 0; i < sizeof(this->Addresses); i++){
    RF24NetworkHeader header(this->Addresses[i]);
    network.write(header, &Message)
  }
  */
  String Message = "VTST;";

  RF24NetworkHeader header;
  network.multicast(header, &Message, sizeof(Message), 1);
}


////////////////////////////////
//======CONTROL FUNCTION======//
////////////////////////////////

void NodeSystem::RUNSYS(){
  //
  // KEY BEGINNING COMMANDS
  // PING -> Pings System
  // PONG -> Response to Ping
  // ATTN -> Attach Node
  // VOTE -> Sending in VOTE
  // PISN -> Ping Second in command
  // UPLD -> Start Upload Early
  // SINC -> Second In Command Update Key
  // REJN -> Rejoin Network Command
  //
  String Message;

  //======RECEIVING======//
  Message = this->Receive();
  //Message = "Hello;World;";
    
  int Idx = Message.indexOf(";");
  String Beginning = Message.substring(0, Idx);

  int Idx2 = Message.indexOf(';', Idx+1 );
  String End = Message.substring(Idx+1, Idx2);

  if (this->this_node == this->MasterNode){
    //////////////////////
    //======MASTER======//
    //////////////////////

    if (Beginning == "PING"){
      //======IS MASTER ALIVE?======//
      Serial.println("Ping Pong");
      this->RespondToPing();
    }
    else if (Beginning == "ATTN"){
      //======ADDING A NODE======//
      Serial.println("Adding Node");
      this->AddNode(End);
    }
    else if (Beginning == "VOTE"){
      //======VOTING======//
      Serial.println("Voting Started");
      if ((this->VotingStarted == false) && (this->VotingEnded == false)){
        this->VoteRecv(End, this->AddressPos, this->VotingStarted, this->VotingEnded);
        this->VotingStarted = true;
      }
      else if ((this->VotingStarted == true) && (this->VotingEnded == false)){
        this->VoteRecv(End, this->AddressPos, this->VotingStarted, this->VotingEnded);
        if (this->VotingPos == this->AddressPos){
          this->VotingEnded = true;
        }
      }
      else if ((this->VotingStarted == true) && (this->VotingEnded == true)){
        this->VoteRecv(End, this->AddressPos, this->VotingStarted, this->VotingEnded);
        this->VotingStarted = false;
        this->VotingEnded = false;
      }
    }
    else if (Beginning == "UPLD"){
      this->Upload(End);
    }
  }
  else if (this->this_node == this->SecondaryNode){
    /////////////////////////////////
    //======SECOND IN COMMAND======//
    /////////////////////////////////
    this->PingMaster();

    delay(1000);

    this->StartVote();
  }
  else if ((this->this_node != this->MasterNode) && (this->this_node != this->SecondaryNode)){
    //////////////////////////
    //======BASIC NODE======//
    //////////////////////////

    this->Receive();

    int Idx = Message.indexOf(";");
    String Beginning = Message.substring(0, Idx);

    if (Beginning == "VTST"){
      this->Voting(16);
    }
    else if (Beginning == "REJN"){
      this->RejoinNetwork();
    }
  }

  this->Upload();
}
