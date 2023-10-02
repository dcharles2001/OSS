#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include <arduino-timer.h>


#define Password_Length 5 
#define RLED 12
#define Alarm 11
#define BTN 13

auto timer = timer_create_default();
auto timer2 = timer_create_default();

int Attempts = 1;
int Criminal = 0;
int CorrectPass = 0; 
int Menu = 1; //Describes the "layer" OF menu we are in
int List = 1;
int Option = 1;
int Seconds = 10;
int x = 0;
int btn = 0;
int RESET = 0;
char Data[Password_Length]; 
char Master[Password_Length] = "123A"; 
byte data_count = 0, master_count = 0;
bool Pass_is_good;
char customKey;
const byte ROWS = 4;
const byte COLS = 4;

char hexaKeys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

byte rowPins[ROWS] = {9, 8, 7, 6};
byte colPins[COLS] = {5, 4, 3, 2};

Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);

LiquidCrystal_I2C lcd(0x27, 16, 2);  

bool countdown(void * /* optional argument given to in/at/every */) {
  if(CorrectPass == 0 && Criminal == 1){
    Seconds--;
    if(Seconds>9){
    lcd.setCursor(12,1);
    }else{
      lcd.setCursor(11,1);
    }
    lcd.print("(");
    lcd.print(Seconds);
    lcd.print(")");
    if (Seconds<0){
      return false;
    }
    }
    return true;
  }

bool Flash(void *){
    Siren();
    return true;
  }
 
        
void setup(){
  lcd.init(); 
  lcd.backlight();
  pinMode(RLED, OUTPUT);
  pinMode(Alarm, OUTPUT);
  pinMode(BTN, INPUT);
  Serial.begin(9600);
  timer.every(1000, countdown);
  timer2.every(250, Flash);
}

void loop(){
  if (RESET == 0){
    if (Criminal == 0){
      if (CorrectPass == 0){
        Menu = 1; //Describes the "layer" OF menu we are in
        List = 1; //Options in menu layer 1
        Option = 1; //Options in menu layer 2
        x = 0;
        lcd.setCursor(0,0);
        lcd.print("Enter Password:");
      
        customKey = customKeypad.getKey();
        if (customKey){
          Data[data_count] = customKey; 
          lcd.setCursor(data_count,1); 
          lcd.print(Data[data_count]); 
          data_count++; 
          }
      
        if(data_count == Password_Length-1){
          lcd.clear();
      
          if(!strcmp(Data, Master)){
            lcd.print("Correct, press #");
            CorrectPass = 1;
            delay(500);
            if(customKey != customKeypad.findInList(14)){
             while(customKey != customKeypad.findInList(14)){
                customKey = customKeypad.getKey();
              }
            }
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
              lcd.print("Attempts left"); 
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
           customKey = customKeypad.getKey();
          if(customKey == customKeypad.findInList(11)){
            while(customKey == customKeypad.findInList(11)){
              customKey = customKeypad.getKey();
              }
              if(List>1){
              List--;
              Serial.print(List);
              lcd.clear();
              }
              
      }else if(customKey == customKeypad.findInList(15)){
             while(customKey == customKeypad.findInList(15)){
                customKey = customKeypad.getKey();
              }
              if(List<3){
              List++;
              Serial.print(List);
              lcd.clear();
              }  
      }else if(customKey == customKeypad.findInList(7)){
            while(customKey == customKeypad.findInList(7)){
              customKey = customKeypad.getKey();
              }
                lcd.clear();
                Menu = 2;
                Option = List + 1; 
              
      }else if(customKey == customKeypad.findInList(3)){
             while(customKey == customKeypad.findInList(3)){
                customKey = customKeypad.getKey();
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
                Siren();
                delay(250);
            }
            lcd.clear();
            lcd.print("Test Over");
            lcd.setCursor(0,1);
            lcd.print("Press * to end");
            back_button();
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
        lcd.setCursor(0,0);
        lcd.print("Enter Password:");
      
        customKey = customKeypad.getKey();
        if (customKey){
          Data[data_count] = customKey; 
          lcd.setCursor(data_count,1); 
          lcd.print(Data[data_count]); 
          data_count++; 
          }
      
        if(data_count == Password_Length-1){
          lcd.clear();
      
          if(!strcmp(Data, Master)){
            lcd.print("Correct, banana");
            CorrectPass = 1;
            Criminal = 0;
            delay(500);
            if(customKey != customKeypad.findInList(14)){
             while(customKey != customKeypad.findInList(14)){
                customKey = customKeypad.getKey();
              }
            }
          }else{
                lcd.clear();
                lcd.print("****ALARMED!****");
                timer.cancel();
                lcd.print("                ");
                timer2.tick();
                RESET = 1;
           }
        }        
      }
    }else{
         btn = digitalRead(BTN);
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
                    lcd.print("Locking");
                    delay(500);
                  lcd.clear();
          }
      }
}
        
      



void Siren(){
    digitalWrite(RLED, !digitalRead(RLED));
    if (digitalRead(RLED) == 1){
      analogWrite(Alarm, 230);
    }else{
      analogWrite(Alarm, 0);
      }
  }


void clearData(){
  while(data_count !=0){
    Data[data_count--] = 0; 
  }
  return;
}
void back_button(){
    customKey = customKeypad.getKey();
       if(customKey != customKeypad.findInList(12)){
        while(customKey != customKeypad.findInList(12)){
                customKey = customKeypad.getKey();
              }
            Menu--;
       }
}
  


  
