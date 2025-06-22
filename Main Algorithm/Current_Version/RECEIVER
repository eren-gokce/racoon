// === RECEIVER ===

#include <Arduino.h>
#include <LoRa_E32.h>
#include <cstring>  // memcpy

HardwareSerial SerialAT(2);
#define LORA_RX 16
#define LORA_TX 17

LoRa_E32 e32ttl(&SerialAT);

struct _attribute_((packed)) Payload {
  uint8_t teamID;
  uint8_t packetCounter;
  float   baroAlt;
  float   gpsAlt;
  float   lat;
  float   lon;
  float   accelX, accelY, accelZ;
  uint8_t status;
};

void setup() {
  Serial.begin(115200);
  SerialAT.begin(9600, SERIAL_8N1, LORA_RX, LORA_TX);
  while (!Serial);
  delay(100);
  e32ttl.begin();
  Serial.println("\n🚀 RX: waiting for Team1 packets…");
}

void loop() {
  if (e32ttl.available() >= sizeof(Payload)) {
    ResponseStructContainer rc = e32ttl.receiveMessage(sizeof(Payload));
    Serial.println(rc.status.getResponseDescription());

    Payload p;
    memcpy(&p, rc.data, sizeof(p));

    Serial.print("RX Team: ");    Serial.println(p.teamID);
    Serial.print("RX #pkt: ");    Serial.println(p.packetCounter);
    Serial.print("RX BaroAlt: "); Serial.println(p.baroAlt);
    Serial.print("RX GPSAlt: ");  Serial.println(p.gpsAlt);
    Serial.print("RX Lat: ");     Serial.println(p.lat, 6);
    Serial.print("RX Lon: ");     Serial.println(p.lon, 6);
    Serial.print("RX Accel: ");   Serial.print(p.accelX,3); Serial.print(","); Serial.print(p.accelY,3); Serial.print(","); Serial.println(p.accelZ,3);
    Serial.print("RX Status: ");  Serial.println(p.status);
    Serial.println();
  }
}
