#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>

#define Password_Length 5 
#define YLED 12

int signalPin = 11;
int Attempts = 3;
int Criminal = 0;

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
  pinMode(signalPin, OUTPUT);
  pinMode(YLED, OUTPUT);
}

void loop(){
  if (Criminal == 0){
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
        lcd.print("Correct");
        digitalWrite(signalPin, HIGH); 
        delay(5000);
        digitalWrite(signalPin, LOW);
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
          digitalWrite(signalPin, HIGH);
          delay(1500);
          digitalWrite(signalPin, LOW); 
        }else{
          lcd.clear();
           Criminal = 1;
           }
        }
        lcd.clear();
        clearData();
      } 
    }else{
          lcd.clear();
          lcd.print("Freeze Dirtbag");
          digitalWrite(signalPin, HIGH);
          digitalWrite(YLED, HIGH);
          delay(1000);
          digitalWrite(signalPin, LOW); 
          digitalWrite(YLED, LOW);
          delay(1000);
    }
}

void clearData(){
  while(data_count !=0){
    Data[data_count--] = 0; 
  }
  return;
}
