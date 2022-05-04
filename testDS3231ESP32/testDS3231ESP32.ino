//Libraries
#include <Wire.h>
#include <DS3231.h>
//Variables
byte Year ;
byte Month ;
byte Date ;
byte DoW ;
byte Hour ;
byte Minute ;
byte Second ;
bool Century  = false;
bool h12 ;
bool PM ;
//Objects
DS3231 Clock;
void setup() {
 //Init Serial USB
 Serial.begin(9600);
 Serial.println(F("Initialize System"));
 Wire.begin();
}
void loop() {
 RTC();
 String dateTime = ""; 
 dateTime += String(Year); 
 dateTime += '-'; 
 dateTime += String(Month); 
 dateTime += '-';
 dateTime += (Date); 
 dateTime += ' ';
 dateTime += String(Hour); 
 dateTime += ':'; 
 dateTime += String(Minute); 
 dateTime += ':'; 
 dateTime += String(Second);
 Serial.println(dateTime);
}
void RTC( ) { /* function readRTC */
 ////Read Real Time Clock
 Year = Clock.getYear();
 Month = Clock.getMonth(Century);
 Date = Clock.getDate();
 Hour = Clock.getHour(h12, PM);
 Minute = Clock.getMinute();
 Second = Clock.getSecond();
 delay(1000);
}
