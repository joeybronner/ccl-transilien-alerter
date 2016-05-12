#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <TM1637Display.h> 

/* 
 *  CLL TRANSILIEN ALERTER
 *  This project use the SNCF APIs
 *  An access is needed, if you want to use it, please email at ext.synergie.florian.lainez@sncf.fr
 *  and 
 *  
 *  --- Board ---
 *  Arduino UNO
 *  Price: ~ 5.00€
 *  
 *  --- 4 Digit Display ---
 *  Croquis > Inclure une bibliothèque > Ajouter la bibliothèque .ZIP > /libs/TM1637.zip
 *  Lib : https://github.com/avishorp/TM1637
 *  Price: ~ 1.00€
 *  
 *  --- LCD 16 x 2 ---
 *  Price: ~ 3.00€
 *  
 *  --- LEDs (green & red) ---
 *  Price: ~ 0.10€
 *  
 *  
 *  Total approximate price : ~ 9.10€
 *  
 */

// 4 Digit Display
// Set the CLK & DIO pin connection to the display
// and set up the device
const int CLK = 8;
const int DIO = 9;
TM1637Display display(CLK, DIO); 

// LCD
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);

// LEDs
int red = 3;
int green = 2;

String travelInfos;

/*
 * Setup description
 */
void setup()
{
  // Communication btw 9600 bauds
  Serial.begin(9600);
  
  // Set the diplay to maximum brightness
  display.setBrightness(0x0a);

  // Initialize LCD map and print welcome message  
  lcd.begin(16,2);
  lcd.setBacklight(50);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("CLICHY/LEVALLOIS");
  lcd.setCursor(0, 1);
  lcd.print(">>>TRANSILIEN<<<");

  // Initialize LEDs
  pinMode(red,OUTPUT);
  pinMode(green,OUTPUT);
}
 
/*
 * Loop description
 */
void loop() 
{
    travelInfos = "";
    while (!Serial.available()) {
      // ... waiting data from serial port (9600 bauds) ...
    }
    
    // serial read section
    while (Serial.available()) {
      delay(30);  // delay to allow buffer to fill 
      if (Serial.available() > 0) {
        char c = Serial.read();   // gets one byte from serial buffer
        travelInfos += c;         // makes the string readString
        
      }
    }

    String minutes      = split(travelInfos, '_', 0);
    String dateTravel   = split(travelInfos, '_', 1);
    String destination  = split(travelInfos, '_', 3);
    String travelStatus = split(travelInfos, '_', 4);

    // Change destination from code to text
    switch (destination.toInt()) {
      case 87384008:
        destination = "PARIS ST/LAZARE";
        break;
      case 87386318:
        destination = "NANTERRE/UNIVERS";
        break;
      case 87382861:
        destination = "VERSAILLES RIV/D";
        break;
      case 87382655:
        destination = "CERGY LE HAUT";
        break;
      case 87386425:
        destination = "MAISONS LAFFITTE";
        break;
      break;
    }

    // Display the minutes remaining for the next departure
    int minLeft = minutes.toInt();
    if (minLeft < 0) {
      minLeft = minLeft * (-1);
    }
    
    display.showNumberDec(minutes.toInt());

    // Update LCD departure informations : date & terminal station
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(dateTravel);
    lcd.setCursor(0, 1);
    lcd.print(destination);

    // Switch on LED red if the travel is canceled, else green
    if (travelStatus == "OK") {
      digitalWrite(green, HIGH);
      digitalWrite(red, LOW); 
    } else {
      digitalWrite(red, HIGH);
      digitalWrite(green, LOW); 
    }
}

/* ----------------------------------------------------------
 * ----------------------------------------------------------
 *                            UTILS
 * ----------------------------------------------------------
 * ----------------------------------------------------------
 */

/*
 * Method description
 */
String split(String data, char separator, int index) {
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length()-1;
  for (int i=0; i<=maxIndex && found<=index; i++){
    if (data.charAt(i)==separator || i==maxIndex){
      found++;
      strIndex[0] = strIndex[1]+1;
      strIndex[1] = (i == maxIndex) ? i+1 : i;
    }
  }
  return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}

