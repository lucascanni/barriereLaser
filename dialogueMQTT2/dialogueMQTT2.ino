#include <ESP32Time.h>
#include <DS3231.h>
#include <PubSubClient.h>
#include <WiFi.h>
#include <Wire.h>

#define _pinBP11 34
#define _pinBP12 35
#define _pinBP21 32
#define _pinBP22 33

unsigned long interBarriere = 0, barriere1 = 0, detectionPonct = 0, barriere2 = 0;
volatile boolean flag = false;
volatile int t11 = 0, t21 = 0, t12 = 0, t22 = 0;
byte Year ;
byte Month ;
byte Date ;
byte DoW ;
byte Hour ;
byte Minute ;
byte Second ;
String strYear = "", strMonth = "", strDate = "", strHour = "", strMinute = "", strSecond = "";
bool Century  = false;
bool h12 ;
bool PM ;
String str_dateTime = "";
String str_dateTime11 = "";
char sz_dateTime[25];
const char* ssid = "wireless_cdf";
const char* password =  "1A2B3C4D5E";
const char* mqtt_server = "192.168.1.20";

//Objects
RTClib myRTC;
ESP32Time rtc;
DateTime now;
IPAddress ip(192, 168, 1, 30);
IPAddress gateway(192, 168, 1, 254);
IPAddress subnet(255, 255, 255, 0);

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  pinMode(_pinBP11, INPUT);
  pinMode(_pinBP12, INPUT);
  pinMode(_pinBP21, INPUT);
  pinMode(_pinBP22, INPUT);
  Serial.begin(115200);
  setup_wifi();
  Wire.begin();
  client.setServer(mqtt_server, 1883);
  attachInterrupt(_pinBP11, appui, RISING);
  attachInterrupt(_pinBP21, appui2, RISING);
  attachInterrupt(_pinBP12, relache, FALLING);
  attachInterrupt(_pinBP22, relache2, FALLING);
}

void setup_wifi(){
  
  delay(4000);   //Delay needed before calling the WiFi.begin
  
  //on oublie l'ancienne config
  WiFi.disconnect(true);
  delay(1000);
  WiFi.mode(WIFI_STA);

  WiFi.config(ip, gateway, subnet);
  WiFi.begin(ssid, password); 
  Serial.println("\nConnecting");
  
  while(WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(100);
  }
  
  Serial.println("\nConnected to the WiFi network");
  Serial.print("[+] ESP32 IP : ");
  Serial.println(WiFi.localIP());
}

void appui() {
  t11 = micros();
  RTC();
}

void relache() {
  t12 = micros();
}

void appui2() {
  t21 = micros();
}

void relache2() {
  t22 = micros();
  flag = true;
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP8266Client","pi","raspberry")) {
      Serial.println("connected");
      RTC();
      verif_RTC();
      if(str_dateTime == NULL){
        str_RTC();
      }
      str_dateTime.toCharArray(sz_dateTime,str_dateTime.length()+1);
      client.publish("laser/init",sz_dateTime);
      Serial.print("Envoi date/heure : ");
      Serial.println(str_dateTime);
      
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void RTC( ) { /* function readRTC */
 ////Read Real Time Clock
 str_dateTime = "";
 now = myRTC.now();
 Year = now.year();
 Month = now.month();
 Date = now.day();
 Hour = now.hour();
 Minute = now.minute();
 Second = now.second();
}

void str_RTC(){
 
 str_dateTime += strYear; 
 str_dateTime += '-'; 
 str_dateTime += strMonth; 
 str_dateTime += '-';
 str_dateTime += strDate; 
 str_dateTime += ' ';
 str_dateTime += strHour; 
 str_dateTime += ':'; 
 str_dateTime += strMinute; 
 str_dateTime += ':'; 
 str_dateTime += strSecond;
}

void verif_RTC(){
  for(unsigned i = 0;i<10;i++){
    if ((Year >= 22 && Year <= 50) || (Year >= 2022 && Year <= 2050)) {
      break;
    }
    else{
      Year = now.year();
    }
  }
    if ((Year < 22 || Year > 50) || (Year < 2022 || Year > 2050)) {
      
        Serial.println("Veuillez entrer manuellement la date et l'heure : ");
        
        Serial.println("Year (YYYY) :");
        Serial.setTimeout(60000);
        strYear = Serial.readStringUntil('\n');
        Serial.print(strYear);
        
        Serial.println("Month (MM) :");
        Serial.setTimeout(60000);
        strMonth = Serial.readStringUntil('\n');
        Serial.print(strMonth);
        
        Serial.println("Date (DD) :");
        Serial.setTimeout(60000);
        strDate = Serial.readStringUntil('\n');
        Serial.print(strDate);
        
        Serial.println("Hour (HH) :");
        Serial.setTimeout(60000);
        strHour = Serial.readStringUntil('\n');
        Serial.print(strHour);
        
        Serial.println("Minute (MM) :");
        Serial.setTimeout(60000);
        strMinute = Serial.readStringUntil('\n');
        Serial.print(strMinute);
        
        Serial.println("Second (SS) :");
        Serial.setTimeout(60000);
        strSecond = Serial.readStringUntil('\n');
        Serial.print(strSecond);
        str_dateTime = rtc.setTime(strSecond.toInt(), strMinute.toInt(), 
        strHour.toInt(), strDate.toInt(), strMonth.toInt(), strYear.toInt());
        str_dateTime = rtc.getTime("%F %T");
    }
    else{
      strYear = String(Year);
      strMonth = String(Month);
      strDate = String(Date);
      strHour = String(Hour);
      strMinute = String(Minute);
      strSecond = String(Second);
      str_RTC();
    }
}

void pubIntervalles(){
  if(flag == true){
   if((WiFi.status()== WL_CONNECTED) 
   && ((t11 > 0) && (t12 > 0) && (t21 > 0) && (t22 > 0)) 
   && ((t21 > t11)&&(t22 > t21)) 
   && ((t21 > t12) && (t22 > t11))){
      interBarriere = t21 - t11;
      barriere1 = t12 - t11;
      detectionPonct = t22 - t11; 
      barriere2 = t22 - t21;
      str_dateTime = rtc.getTime("%F %T");
      str_dateTime += " ";
      str_dateTime += String(interBarriere);
      str_dateTime += " ";
      str_dateTime += String(barriere1);
      str_dateTime += " ";
      str_dateTime += String(detectionPonct);
      str_dateTime += " ";
      str_dateTime += String(barriere2);
      str_dateTime.toCharArray(sz_dateTime,str_dateTime.length()+1);
      client.publish("laser/intTemps",sz_dateTime);
      Serial.println(sz_dateTime);
    }else{
    
      Serial.println("Error in WiFi connection");   
    
   }
    
    flag = false;
  }
}

void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  
  pubIntervalles();
}
