#include <Arduino.h>
#include <LoRa_E32.h>
#include <cstring>  // memcpy

HardwareSerial SerialAT(2);
#define LORA_RX 16
#define LORA_TX 17

LoRa_E32 e32ttl(&SerialAT);

#pragma pack(push,1)
struct Payload {
  uint8_t teamID;
  uint8_t packetCounter;
  float   baroAlt;
  float   gpsAlt;
  float   lat;
  float   lon;
  float   lora_yaw;
  float   lora_pitch;
  float   lora_row;
  float   accelX;
  float   accelY;
  float   accelZ;
  float   speed;
  uint8_t status;
};
#pragma pack(pop)

void setup() {
  Serial.begin(9600);
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

    Serial.print("TX bytes total: "); Serial.println(p.packetCounter);
  Serial.print("  BaroAlt: ");   Serial.println(p.baroAlt);
  Serial.print("  GPSAlt: ");    Serial.println(p.gpsAlt);
  Serial.print("  Lat: ");       Serial.println(p.lat,6);
  Serial.print("  Lon: ");       Serial.println(p.lon,6);
  Serial.print("  Yaw: ");       Serial.println(p.lora_yaw);
  Serial.print("  Pitch: ");       Serial.println(p.lora_pitch);
  Serial.print("  row: ");       Serial.println(p.lora_row);

  Serial.print("  Accel : "); 
    Serial.print(p.accelX,3); Serial.print(", ");
    Serial.print(p.accelY,3); Serial.print(", ");
    Serial.println(p.accelZ,3);
  
  Serial.print("  Speed: ");       Serial.println(p.speed);

  Serial.print("  Status: ");    Serial.println(p.status);
  }
}