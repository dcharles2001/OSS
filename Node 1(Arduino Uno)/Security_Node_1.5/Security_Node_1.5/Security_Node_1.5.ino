#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
//#include <Keypad.h>
#include "Adafruit_Keypad.h"
#include <arduino-timer.h>
#include <ezBuzzer.h>
#include <SoftwareSerial.h>
#define Password_Length 5 
#define RLED 12
#define BTN 13
#define disarmSig A3
#define armSig A0
SoftwareSerial ESPSerial(8,9);

auto timer = timer_create_default();
int Attempts = 3;
int Criminal = 0;
int CorrectPass = 0; 
int Menu = 1; //Describes the "layer" OF menu we are in
int List = 1;
int Option = 1;
int Seconds = 10;
int x = 0;
int btn = 0;
int RESET = 0;
const int Alarm = 11;
char Data[Password_Length]; 
char Master[Password_Length] = "123A"; 
byte data_count = 0, master_count = 0;
bool Pass_is_good;
keypadEvent customKey;
const byte ROWS = 4;
const byte COLS = 4;
int disarm = 0;
int arm = 0;
char hexaKeys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

int tune[] = {
  NOTE_G4,NOTE_C4,NOTE_G5,
  NOTE_C5
  };
int noteDurations[] = {
    6, 6 ,6 ,6
  };
byte rowPins[ROWS] = {7, 6, 5, 4};
byte colPins[COLS] = {3, 2, 13, 12};

Adafruit_Keypad customKeypad = Adafruit_Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);

LiquidCrystal_I2C lcd(0x27, 16, 2); 

ezBuzzer buzzer(Alarm);

int length = sizeof(noteDurations) / sizeof(int);

bool countdown(void * /* optional argument given to in/at/every */) {
  if(CorrectPass == 0 && Criminal == 1 ){
    Seconds--;
    if(Seconds>9){
    lcd.setCursor(12,1);
    }else{
      lcd.setCursor(13,1);
    }
    if (Seconds>0){
      lcd.print("(");
      lcd.print(Seconds);
      lcd.print(")");
    }if(Seconds == 0){
         lcd.clear();
      }else if (Seconds < 0){
      lcd.setCursor(0,0);
      lcd.print("****ALARMED!****");  
      Flash();
      buzzer.playMelody(tune, noteDurations, length);
      }
  }
  return true;
}

 void(*resetFunc) (void) = 0;
        
void setup(){
  ESPSerial.begin(9600);
  lcd.init(); 
  lcd.backlight();
  pinMode(RLED, OUTPUT);
  pinMode(Alarm, OUTPUT);
  pinMode(BTN, INPUT);
  Serial.begin(9600);
  timer.every(1000, countdown);
  customKeypad.begin();
  //attachInterrupt(armSig, Print, RISING);
}

void loop(){
  buzzer.loop();
  customKeypad.tick();
  arm = analogRead(armSig);
  Serial.print("Arm = ");
  Serial.println(arm);
  disarm = analogRead(disarmSig);
  Serial.print("Disarm = ");
  Serial.println(disarm);
  if(arm > 100){
    Criminal = 1;
    CorrectPass = 0;
    Seconds = 0;
    timer.tick();
    }else if(disarm > 100){
      resetFunc();
      }
    
  if (RESET == 0){
    if (Criminal == 0){
      if (CorrectPass == 0){
        digitalWrite(RLED, 0);
        Menu = 1; //Describes the "layer" OF menu we are in
        List = 1; //Options in menu layer 1
        Option = 1; //Options in menu layer 2
        x = 0;
        lcd.setCursor(0,0);
        lcd.print("Enter Password:");
      
        customKey = customKeypad.read();
        if (customKey.bit.EVENT == KEY_JUST_PRESSED){
          Data[data_count] = (char)customKey.bit.KEY; 
          lcd.setCursor(data_count,1); 
          lcd.print(Data[data_count]); 
          data_count++; 
          }
      
        if(data_count == Password_Length-1){
          lcd.clear();
      
          if(!strcmp(Data, Master)){
            CorrectPass = 1;
            customKeypad.clear();
            delay(500);
          }
          else{
            lcd.print("Incorrect");
            delay(1000);
            Attempts--;
            if (Attempts>0){
              lcd.clear();
              lcd.print("You have (");
              lcd.print(Attempts);
              lcd.print(")");
              lcd.setCursor(0,1); 
              lcd.print("Attempt(s) left"); 
              delay(2000);
            }else{
              lcd.clear();
               Criminal = 1;
               }
            }
            lcd.clear();
            clearData();
          }
      }else{
         if (x == 0){
            lcd.clear();
            lcd.setCursor(0,0); 
            lcd.print("Welcome Home");
            lcd.setCursor(0,1);
            lcd.print("Robin DaHouse");
            Attempts = 3;
            delay(1500);
            lcd.clear();
            x = 1;
         }
   switch(Menu){
         case 1:
            switch (List){
              case 1:
                lcd.setCursor(0,0);
                lcd.print("A)Lock"); 
                lcd.setCursor(0,1);
                lcd.print("B)Network Test");
                break;
              case 2:
                lcd.setCursor(0,0);
                lcd.print("A)Network Test"); 
                lcd.setCursor(0,1);
                lcd.print("B)Alarm Test");
                break;
              case 3:
                lcd.setCursor(0,0);
                lcd.print("A)Alarm Test"); 
                lcd.setCursor(0,1);
                lcd.print("B)Support");
               break;
              default:
              break;
            }
          customKey = customKeypad.read();
          if ((customKey.bit.EVENT == KEY_JUST_PRESSED) && (customKey.bit.KEY == 'C')){
            if (List>1){
            List--;
            Serial.print(List);
            lcd.clear();
          }
          

      }else if((customKey.bit.EVENT == KEY_JUST_PRESSED) && (customKey.bit.KEY == 'D')){
              if(List<3){
              List++;
              Serial.print(List);
              lcd.clear();
              }  
      }else if((char)customKey.bit.KEY == 'B'){
            while((char)customKey.bit.KEY == 'B'){
              customKey = customKeypad.read();
              }
                lcd.clear();
                Menu = 2;
                Option = List + 1; 
              
      }else if((char)customKey.bit.KEY == 'A'){
             while((char)customKey.bit.KEY == 'A'){
                customKey = customKeypad.read();
              }
                lcd.clear();
                Menu = 2;
                Option = List;
        } 
      break;
      case 2:
        switch(Option){
          case 1:
              Attempts = 3;
              Criminal = 0;
              CorrectPass = 0; 
              Menu = 1;
              List = 1;
              Option = 1;
              Seconds = 10;
              x = 0;
              data_count = 0, master_count = 0;
              lcd.clear();
              lcd.print("Locking");
              delay(500);
              lcd.clear();
          break;
          case 2:
            lcd.setCursor(0,0);
            lcd.print("2");
            back_button();
          break;
          case 3:
            lcd.setCursor(0,0);
            lcd.print("Alarm Test");
            delay(1000);
            for(int i = 0; i<6; i++){
                Flash();
                analogWrite(Alarm, 230);
                delay(125);
                analogWrite(Alarm, 0);
                delay(125);
            }
            lcd.clear();
            lcd.print("Test Over");
            customKeypad.clear();
            Menu = 1;
            delay(500);
          break;
          case 4:
            lcd.setCursor(0,0);
            lcd.print("(029)2078 9418");
            lcd.setCursor(0,1);
            lcd.print("www.OSS.co.scam");
            back_button();
          break;
      break;
        }
   }
      }
      }else{
        timer.tick();
        if(RESET == 0){
          lcd.setCursor(0,0);
          lcd.print("Enter Password:");
        }
        customKey = customKeypad.read();
        if ((customKey.bit.EVENT == KEY_JUST_PRESSED) && (Seconds > 0)){
          Data[data_count] = customKey.bit.KEY; 
          lcd.setCursor(data_count,1); 
          lcd.print(Data[data_count]); 
          data_count++; 
          }
      
        if(data_count == Password_Length-1){
          lcd.clear();
      
          if(!strcmp(Data, Master) && (Seconds > 0)){
            CorrectPass = 1;
            Criminal = 0;
            customKeypad.clear();
            delay(500);
            /*
            if((char)customKey.bit.KEY != '#'){
             while((char)customKey.bit.KEY != '#'){
                customKey = customKeypad.read();
              }
            }
            */
        }else{
                RESET = 1;
                Seconds = 0;
                lcd.clear();
        }         
      }
      if (Seconds == 0){
          RESET = 1;
          lcd.clear();
        }
      
     }
    }else{
      timer.tick();
         btn = 0;
                  if(btn == 1){
                    RESET = 0;
                    Attempts = 3;
                    Criminal = 0;
                    CorrectPass = 0; 
                    Menu = 1;
                    List = 1;
                    Option = 1;
                    Seconds = 10;
                    x = 0;
                    data_count = 0, master_count = 0;
                    lcd.clear();
                    lcd.print("Resetting");
                    delay(750);
                    lcd.clear();
                    btn = 0;
          }
      }
}
        
      



void Flash(){
    ESPSerial.println("Alarmed");
    disarm = analogRead(disarmSig);
    arm = analogRead(armSig);
    if ((disarm > 100) && (arm < 100)){
      resetFunc();
    }
  }


void clearData(){
  while(data_count !=0){
    Data[data_count--] = 0; 
  }
  return;
}
void back_button(){
    customKey = customKeypad.read();
       if((customKey.bit.EVENT == KEY_JUST_PRESSED) && (customKey.bit.KEY == '*')){
            lcd.clear();
            Menu--;
       }
}

  void Print(){
    Serial.println("AHHHHHH!!");
    delay(100);
  }


  
