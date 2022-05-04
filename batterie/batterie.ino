const int BATTERYPIN = A0; //pin de la batterie

const float TensionMin = 3.2; //tension min
const float TensionMax = 4.2; //tension max

void setup() {
  Serial.begin(9600);
}

int getBattery ()
{
  float b = analogRead(BATTERYPIN); //valeur analogique

  int minValue = (1023 * TensionMin) / 5; //Arduino
  int maxValue = (1023 * TensionMax) / 5; //Arduino

  //int minValue = (4095 * TensionMin) / 3; //ESP32
  //int maxValue = (4095 * TensionMax) / 3; //ESP32

  b = ((b - minValue) / (maxValue - minValue)) * 100; //mettre en pourcentage

  if (b > 100) //max is 100%
    b = 100;

  else if (b < 0) //min is 0%
    b = 0;
  int valeur = b;
  return b;
}

void loop() {
  Serial.println(getBattery());
  delay(1000);
}
