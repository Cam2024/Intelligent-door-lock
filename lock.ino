#include <Keypad.h>  //4*4keypad
#include <LiquidCrystal.h>  //LCD
#include <Servo.h>  //micro servo

#define Password_Length 5
Servo myservo;
LiquidCrystal lcd(A0, A1, A2, A3, A4, A5);
int distance;
int light_switch;
int last=2;
int status=0;
int pos = 0;
char Data[Password_Length];
char Master[Password_Length] = "1234";
byte data_count = 0, master_count = 0;
char customKey;

/*---preparing keypad---*/
const byte ROWS = 4;
const byte COLS = 4;
char keys[ROWS][COLS] = 
{
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
byte rowPins[ROWS] = {7, 6, 5, 4};
byte colPins[COLS] = {3, 2, 1, 0};
Keypad customKeypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS);

/*--- Main Action ---*/
void setup()
{
  pinMode(10,OUTPUT);
  pinMode(11,OUTPUT);
  pinMode(12,INPUT);
  pinMode(13,INPUT);
  myservo.attach(8, 2000, 2400);
  ServoClose();
  lcd.begin(16, 2);
  lcd.print("Protected Door");
  loading("Loading");
  lcd.clear();
}

void loop()
{
  dis();
  led();
  if(distance<50)
  {
    Open();
  }
  else
  {
    lcd.clear();
    clearData();
  }
}

void dis() //get distance
{
  digitalWrite(11,LOW);
  delayMicroseconds(5);
  digitalWrite(11,HIGH);
  delayMicroseconds(10);
  digitalWrite(11,LOW);
  distance = pulseIn(12,HIGH)*0.0173;
}

void led()  //get switch and control light bulb
{
  light_switch=digitalRead(13);
  if(last!=2)
  {
    if(last!=light_switch)
    {
      if(status==0)
      {
        digitalWrite(10,HIGH);
        status=1;
      }
      else
      {
        digitalWrite(10,LOW);
        status=0;
      }
    }
  }
  last=light_switch;
}

void loading (char msg[]) // Display waiting
{
  lcd.setCursor(0, 1);
  lcd.print(msg);
  for (int i = 0; i < 9; i++) 
{
    delay(100);
    lcd.print(".");
  }
}

void clearData()  //clear data
{
  while (data_count != 0)
  { 
    Data[data_count--] = 0;
  }
  return;
}

void ServoClose()  //lock door
{
  for (pos = 90; pos >= 0; pos -= 10) { 
    myservo.write(pos);
  }
}

void ServoOpen()  //unlock door
{
  for (pos = 0; pos <= 90; pos += 10) {
    myservo.write(pos);  
  }
}

void Open()  //main action and record password
{
  lcd.setCursor(0, 0);
  lcd.print("Enter Password");
  customKey = customKeypad.getKey(); //get key
  if (customKey)
  {
    Data[data_count] = customKey;
    lcd.setCursor(data_count, 1);
    lcd.print(Data[data_count]);
    data_count++;
  }

  if (data_count == Password_Length - 1)  //judge length
  {
    if (!strcmp(Data, Master))  //judge password
    {
      if(digitalRead(9))   //judge ambient light
      {
      	digitalWrite(10,HIGH);
      }
      lcd.clear();
      ServoOpen();
      lcd.print(" Door is Open ");
      delay(3000);
      loading("Waiting");
      lcd.clear();
      lcd.print(" Time is up! ");
      delay(1000);
      ServoClose();  
    }
    else
    {
      lcd.clear();
      lcd.print(" Wrong Password ");
    }
    delay(1000);
    lcd.clear();
    clearData();
  }
}
