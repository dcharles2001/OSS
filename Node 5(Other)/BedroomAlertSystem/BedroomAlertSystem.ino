// Bedroom ALERT Panel
/*
 * This panel connects to the system and alerts the user if the system is set off
 * It will run over the nrf24L01 system
 */

// Libraries Required
#include <nRF24L01.h>
#include <printf.h>
#include <RF24.h>
#include <RF24_config.h>

RF24 RadioSystem(7, 8);

struct DataPackage{
  int id = 1;
  float Alert = 0;
  char Text[100] = "No Alerets at the Moment";
}


typedef struct DataPackage package

package Data;

void setup() {
  Serial.begin(9600);
  delay(1000);

  RadioSystem.begin();
  RadioSystem.setChannel(115);
  RadioSystem.setPALevel(RF25_PA_MAX);
  RadioSystem.setDataRate(RF24_250KBPS);
  RadioSystem.openWritingPipe(addresses[0]);
  delay(1000);
}

void loop() {
  Data.Text = "ALERT - INTRUDER";
  Data.id = Data.id + 1;


  RadioSystem.write(&Data, sizeof(Data);
  delay(1000);
}
