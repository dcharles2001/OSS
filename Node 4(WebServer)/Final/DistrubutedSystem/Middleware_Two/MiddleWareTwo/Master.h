#ifndef Master_H
#define Master_H

#include <SPI.h>
#include <RF24Network.h>
#include <RF24.h>
#include <Arduino.h>

class NodeSystem{
  private:
    bool MemberofNetwork = false; 
    uint8_t VotingValue = 0;
    unsigned int VotingPos = 0;
    unsigned int AddressPos = 0;
    uint8_t ListOfVotes[8];
    uint8_t Addresses[8];
    uint8_t AmountOfEachVote[8];

    bool VotingStarted = false;
    bool VotingEnded = false;

  public:
    uint16_t this_node = 03;
    uint16_t MasterNode = 00;
    uint16_t SecondaryNode = 01;
  
    void JoinNetwork(); //
    void RejoinNetwork();
    void Voting(int nodenum); //
    void N_TO_M_Update(String Update); 
    void UpdateToSecondinCommand(); //
    String Receive(); ///

    // second in command Functions
    void UpdateToMaster(); //
    void PingMaster(); //
    void CommandRejoinNetwork();

    // Master Only Functions
    void M_TO_N_Update(String Update);
    void Upload(String message = ""); // call this one at the end of all code
    void VoteRecv(String message, int nodenum, bool votestart, bool votecomplete); ///
    void AddNode(String message); ///
    void RespondToPing(); ///
    void StartVote(); ///

    // Control Function
    void RUNSYS();
};

#endif // Master_H