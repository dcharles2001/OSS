#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>

#define Password_Length 5 
#define RLED 12
#define Movement 10

int Attempts = 3;
int Criminal = 0;
int CorrectPass = 0;
int List = 1;
int x = 0;

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

void setup(){
  lcd.init(); 
  lcd.backlight();
  pinMode(RLED, OUTPUT);
  Serial.begin(9600);
}

void loop(){
  if (Criminal == 0){
    if (CorrectPass == 0){
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
          lcd.setCursor(0,0); 
          lcd.print("Welcome Home");
          lcd.setCursor(0,1);
          lcd.print("Robin DaHouse");
          lcd.clear();
          x = 1;
      }
      
          switch (List){
            case 1:
              lcd.setCursor(0,0);
              lcd.print("A)Lock"); 
              lcd.setCursor(0,1);
              lcd.print("B)Unlock");
              break;
            case 2:
              lcd.setCursor(0,0);
              lcd.print("A)Unlock"); 
              lcd.setCursor(0,1);
              lcd.print("B)Brechdanua");
              break;
            case 3:
              lcd.setCursor(0,0);
              lcd.print("A)Brechdanua"); 
              lcd.setCursor(0,1);
              lcd.print("B)Cynio");
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
              lcd.setCursor(0,0);
              lcd.print("*****ACTIVE*****");
              List = 4;
            
    }else if(customKey == customKeypad.findInList(3)){
           while(customKey == customKeypad.findInList(3)){
              customKey = customKeypad.getKey();
            }
              lcd.setCursor(0,1);
              lcd.print("*****ACTIVE*****");
            List = 4;
      } 
   
    }
    
    }else{
          lcd.clear();
          lcd.print("Freeze Dirtbag!");
          digitalWrite(RLED, HIGH);
          delay(500); 
          digitalWrite(RLED, LOW);
          lcd.clear();
          delay(500);
    }
}

void clearData(){
  while(data_count !=0){
    Data[data_count--] = 0; 
  }
  return;
}
