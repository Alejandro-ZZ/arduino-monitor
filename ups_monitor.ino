//-----------------------------------------------------------//
//                      LIBRARIES                            //
//-----------------------------------------------------------//

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Adafruit_ADS1X15.h>
#include <SPI.h>
#include "RTClib.h"
#include "SdFat.h"

/*
 * The following lines are needed because early versions (pre 1.0) 
 * had a Serial method "print" which allowed binary when used
 * with the qualifier BYTE, but this has now been superseded
 * by the "write" method.
 */
#if defined(ARDUINO) && ARDUINO >= 100
  #define printByte(args)  write(args);
#else
  #define printByte(args)  print(args,BYTE);
#endif

//-----------------------------------------------------------//
//                      VARIABLES                            //
//-----------------------------------------------------------//

SdFat SD;                             // MicroSD object
RTC_DS3231 rtc;                       // RTC object
Adafruit_ADS1115 ads;                 // ADS1115 object
LiquidCrystal_I2C lcd(0x27, 16, 2);   // LCD display object

float multiplier = 0.1875F;           // ADS1115 Gain
int16_t shunt_bat, shunt_dc_dc;       // Values to read from ADS1115
float bat = 0;                        // Variable to save battery voltage
float current = 0;                    // Variable to save battery current

const int chipSelect = 10;            // For the CS pin of the microSD module
unsigned long previousMillis = 0;     // Previous time
unsigned long interval = 30000;       // Period of samples (in ms)
String fecha;                         // Date read from RTC
String hora;                          // Time read from RTC

File TimeFile, Logs;                  // File objects for the microSD card

// Thermometer and battery icons
uint8_t Temp[8] = {0x4, 0xA, 0xA, 0xA, 0xE, 0x1F, 0x1F, 0xE};
uint8_t Bat[8] = {0xE, 0x1B, 0x11, 0x11, 0x1F, 0x1F, 0x1F, 0x1F};

//  +-------------------------------------------------------------------------+
//  |                                                                         |
//  |                                   SETUP                                 |
//  |                                                                         |    
//  +-------------------------------------------------------------------------+

//-----------------------------------------------------------//
//                     Code to run once                      //
//-----------------------------------------------------------//
void setup() {
  
  lcd.init();
  lcd.backlight();
  lcd.print("Starting...");

  if (!SD.begin(chipSelect)) {
    lcd.setCursor(0,1); // (col, row)
    lcd.print("                ");
    lcd.setCursor(0,1); // (col, row)
    lcd.print("Error SD");
    delay(2000);
  }

  ads.begin();

  if (!rtc.begin()) {
    lcd.setCursor(0,1); // (col, row)
    lcd.print("                ");
    lcd.setCursor(0,1); // (col, row)
    lcd.print("Error RTC");
    delay(2000);
  }

  // Ads "Reset System" Log to microSD card
  Logs = SD.open("LOGS.txt", FILE_WRITE);
    if (Logs) {
      timeValues();                   // Gets date and time
      Logs.print(fecha);
      Logs.print(",");
      Logs.print(hora);
      Logs.print(",");
      Logs.println("Reset System");
      Logs.close();
    }

    // Creates icons for LCD display
    lcd.createChar(0, Temp);
    lcd.createChar(1, Bat);
    
    lcd.setCursor(0,1);             // (col, row)
    lcd.print("                ");
    lcd.setCursor(0,1);
    lcd.print("DONE!");
    
    delay(2000);
    
    lcd.clear();
    lcd.home();
    lcd.printByte(1);   // Display battery icon
    lcd.print(":");
    lcd.setCursor(9,0);
    lcd.print("Rs:");
    lcd.setCursor(0,1);
    lcd.print("Io:");
    lcd.setCursor(9,1);
    lcd.printByte(0);   // Display thermometer icon

    recordData();       // Records data to microSD and LCD display
}

//  +-------------------------------------------------------------------------+
//  |                                                                         |
//  |                                  LOOP                                   |
//  |                                                                         |    
//  +-------------------------------------------------------------------------+

//-----------------------------------------------------------//
//                  Code to run repeatedly                   //
//-----------------------------------------------------------//
void loop() {
  
  unsigned long currentMillis = millis();           // Gets current time

  // Reads and update data every 'interval' time  
  if (currentMillis - previousMillis >= interval){

    previousMillis = currentMillis;                 // Update previous time
    recordData();                                   // Records data to microSD and LCD display
  }
}

//  +-------------------------------------------------------------------------+
//  |                                                                         |
//  |                               FUNCTIONS                                 |
//  |                                                                         |    
//  +-------------------------------------------------------------------------+

//-----------------------------------------------------------//
//              Refresh and display data in LCD              //
//-----------------------------------------------------------//
void showLCD(){
  
  // Battery Voltage (in V)
  lcd.setCursor(2,0);
  lcd.print("     ");
  lcd.setCursor(2,0);
  lcd.print(bat);
  
  // Shunt resistor sensing voltage (in mV)
  lcd.setCursor(12,0);
  lcd.print("    ");
  lcd.setCursor(12,0);
  lcd.print(shunt_dc_dc);

  // Temperature (in °C)
  lcd.setCursor(10,1);
  lcd.print("   ");
  lcd.setCursor(10,1);
  lcd.print(rtc.getTemperature());

  // Battery Current (in A)
  lcd.setCursor(3,1);
  lcd.print("     ");
  lcd.setCursor(3,1);
  lcd.print(current);
}

//-----------------------------------------------------------//
//                    Reads ADC values                       //
//-----------------------------------------------------------//
void adcValues(){

  // Battery voltage divider measurement
  shunt_bat = ads.readADC_Differential_0_1() * multiplier;
  
  // Shunt resistor sensing measurement
  shunt_dc_dc = ads.readADC_Differential_2_3() * multiplier;
}

//-----------------------------------------------------------//
//            Reads date and time values from RTC            //
//-----------------------------------------------------------//
void timeValues(){
  DateTime now = rtc.now();

  fecha = String(String(now.day()) + "/" + String(now.month()) + "/" + String(now.year()));
  hora = String(String(now.hour()) + ":" + String(now.minute()) + ":" + String(now.second()));
}

//-----------------------------------------------------------//
//          Records data to microSD and LCD display          //
//-----------------------------------------------------------//
void recordData(){
  
  adcValues();                              // Reads ADC values
  timeValues();                             // Read time and date values

  bat = float(shunt_bat)/1000;              // Converts mV to V
  bat = 3.0088*bat - 0.0055;                // Gets battery voltage
  current = float(shunt_dc_dc)/1000 / 0.05; // Gets current of the battery (sensing resistance: 0.05Ω)
  
  showLCD();                                // Prints info in LCD display
  
  // Save data in microSD in CSV format
  TimeFile = SD.open("DATA.txt", FILE_WRITE);
  if (TimeFile) {
    TimeFile.print(fecha);
    TimeFile.print(",");
    TimeFile.print(hora);
    TimeFile.print(",");
    TimeFile.print(bat);
    TimeFile.print(",");
    TimeFile.print(shunt_dc_dc);
    TimeFile.print(",");
    TimeFile.print(current);
    TimeFile.print(",");
    TimeFile.println(rtc.getTemperature());
    TimeFile.close();
  }
}
