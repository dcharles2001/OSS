#ifndef Master_H
#define Master_H

#include <SPI.h>
#include <RF24Network.h>
#include <RF24.h>

class NodeSystem{
  private:
    bool MemberofNetwork = false;
    uint16_t this_node = 0;
    uint16_t MasterNode = 0;
    uint16_t SecondaryNode = 0;

    uint8_t VotingValue = 0;

  public:
    bool JoinNetwork();
    void Voting();
    void N_TO_M_Update(char *Update);

    // second in command Functions
    void PassToMaster(char *Message);

    // Master Only Functions
    void M_TO_N_Update(char *Update);
    void Upload();
    void VoteRecv();
};

#endif // Master_H