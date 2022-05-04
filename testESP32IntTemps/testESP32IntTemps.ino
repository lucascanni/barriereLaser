#include <Arduino.h>
#define _pinBP1 34
#define _pinBP12 35
#define _pinBP2 32
unsigned long interBarriere = 0, barriere1 = 0;
volatile boolean changed = false;
volatile int t11 = 0, t12 = 0, t21 = 0;

void setup()
{
  pinMode(_pinBP1, INPUT);
  pinMode(_pinBP2, INPUT);
  pinMode(_pinBP12, INPUT);
  Serial.begin(9600);
  attachInterrupt(_pinBP1, appui1, RISING);
  attachInterrupt(_pinBP2, appui2, RISING);
  attachInterrupt(_pinBP12, relache, FALLING);
}

void appui1() {
  t11 = millis();
  Serial.print("t11 = ");
  Serial.println(t11);
}

void relache() {
  t12 = millis();
  Serial.print("t12 = ");
  Serial.println(t12);
}

void appui2() {
  changed = true;
  t21 = millis();
  Serial.print("t21 = ");
  Serial.println(t21);
}

void loop()
{
  if (changed){
    changed = false;
    if(((t11 > 0) && (t12 > 0) && (t21 > 0)) 
   && ((t21 > t11)&&(t12 > t11) && (t21 > t12))){
      interBarriere = t21-t11;
      barriere1 = t12 - t11;
      Serial.print("t21 - t11 = ");
      Serial.print(interBarriere);
      Serial.println(" ms");
      Serial.print("t12 - t11 = ");
      Serial.print(barriere1);
      Serial.println(" ms");
    }
  }
}
