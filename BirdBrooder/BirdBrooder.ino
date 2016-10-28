#include <OneWire.h>

#define RELAY1 4 // Relay on pin 4
const float TEMPLOW = 28.00; // minimum temperature
const float TEMPHI = 35.00; // maximum temperature
OneWire  ds(3); // DS18B20 on pin 3
bool lightOn; // light state

void setup() {
  pinMode(RELAY1, OUTPUT); 
  Serial.begin(9600);
  digitalWrite(RELAY1, LOW); // turn relay off to be safe
  lightOn = false; // update light state
  Serial.println("BirdBrooder v1.0 - boot up complete.");
}


void loop() {
  byte i;
  byte present = 0;
  byte type_s;
  byte data[12];
  byte addr[8];
  float celsius;
  if (!ds.search(addr)) {
    ds.reset_search();
    delay(250);
    return;
  }
  ds.reset();
  ds.select(addr);
  ds.write(0x44, 1);
  delay(1000);
  present = ds.reset();
  ds.select(addr);
  ds.write(0xBE);
  for (i = 0; i < 9; i++) {
    data[i] = ds.read();
  }
  int16_t raw = (data[1] << 8) | data[0];
  byte cfg = (data[4] & 0x60);
  if (cfg == 0x00) raw = raw & ~7; 
  else if (cfg == 0x20) raw = raw & ~3;
  else if (cfg == 0x40) raw = raw & ~1;
  celsius = (float)raw / 16.0;

  Serial.print(celsius);
  Serial.println("C");

  if (celsius <= TEMPLOW) {
    if (!lightOn) {
      lightOn = true;
      Serial.println("Temperature dropped below threshold: Enabling light");
      digitalWrite(RELAY1, HIGH);
    }
  }

  if (celsius >= TEMPHI) {
    if (lightOn) {
      lightOn = false;
      Serial.println("Temperature rose above threshold: Disabling light");
      digitalWrite(RELAY1, LOW);
    }
  }
}
