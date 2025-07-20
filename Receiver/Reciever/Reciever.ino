#include <Arduino.h>
#include <LoRa_E32.h>
#include <cstring>  // memcpy
#include "driver/uart.h"

HardwareSerial SerialAT(2);
#define LORA_RX 16
#define LORA_TX 17

#pragma pack(push,1)
struct Rocket {
  uint8_t id;
  float   baroAlt;
  float   gpsAlt;
  float   lat;
  float   lon;
  float   lora_pitch;
  float   lora_roll;
  float   lora_yaw;
  float   accelX;
  float   accelY;
  float   accelZ;
  float   degree;
  float   speed; // ek benim istedigim bilgi
  uint8_t status;
  uint8_t crc;
};
#pragma pack(pop)
#pragma pack(push,1)
struct Payload {
  uint8_t id;
  float   baroAlt;
  float   gpsAlt;
  float   lat;
  float   lon;
  float   pressure;
  float   temp;
  float   humidity;
  uint8_t crc;
};
#pragma pack(pop)


uint8_t calculateCRC8(const uint8_t* data, size_t len) {
  uint8_t crc = 0;
  for (size_t i = 0; i < len; i++) {
    crc ^= data[i];
    for (uint8_t b = 0; b < 8; b++) {
      if (crc & 0x80)
        crc = (crc << 1) ^ 0x31;
      else
        crc <<= 1;
    }
  }
  return crc;
}

#define BUFFER_SIZE 256
uint8_t fifoBuffer[BUFFER_SIZE];
uint8_t fifoHead = 0;
uint8_t fifoTail = 0;

void fifoPush(uint8_t b) { // fifo ya byte ekleme
  fifoBuffer[fifoHead] = b;
  fifoHead = (fifoHead + 1) % BUFFER_SIZE;

  // FIFO dolarsa, en eski veriyi ez
  if (fifoHead == fifoTail) {
    fifoTail = (fifoTail + 1) % BUFFER_SIZE;
  }
}

bool fifoPop(uint8_t &b) { // fifo byte okuma
  if (fifoHead == fifoTail) return false; // boş
  b = fifoBuffer[fifoTail];
  fifoTail = (fifoTail + 1) % BUFFER_SIZE;
  return true;
}

uint8_t fifoAvailable() {  // fifo veri uzunlugu
  return (fifoHead + BUFFER_SIZE - fifoTail) % BUFFER_SIZE;
}

void parseFifo() {
  if (fifoAvailable() >= 48) {
    uint8_t startByte;
    startByte = fifoBuffer[fifoTail];
    // fifoPop(startByte);

    if (startByte == 131 && fifoAvailable() >= sizeof(Payload)) {
      // Payload mesajı
      Payload p;
      for (int i = 0; i < sizeof(Payload); i++) {
        fifoPop(((uint8_t*)&p)[i]);  // baştan itibaren oku
      }
      Serial.println("📩 Payload alındı:");

      Serial.print("  BaroAlt: ");   Serial.println(p.baroAlt);
      Serial.print("  GPSAlt: ");    Serial.println(p.gpsAlt);
      Serial.print("  Lat: ");       Serial.println(p.lat,6);
      Serial.print("  Lon: ");       Serial.println(p.lon,6);
      Serial.print("  pressure: ");       Serial.println(p.pressure);
      Serial.print("  temp: ");       Serial.println(p.temp);
      Serial.print("  humidity: ");       Serial.println(p.humidity);
      Serial.print("  crc: ");       Serial.println(p.crc);
      uint8_t calc_crc = calculateCRC8((uint8_t*)&p, sizeof(Payload) - sizeof(p.crc));
      Serial.print("calc_crc = "); Serial.println(calc_crc);
      if (p.crc == calc_crc){
        Serial.println("  crc: Dogru");
      }
      else{
        Serial.println("  crc: Yanlis");
      }

    } 
    else if (startByte == 151 && fifoAvailable() >= sizeof(Rocket)) {
      // Rocket mesajı
      Rocket r;
      for (int i = 0; i < sizeof(Rocket); i++) {
        fifoPop(((uint8_t*)&r)[i]);
      }
      Serial.println("🚀 Rocket alındı:");

      Serial.print("  id: ");        Serial.println(r.id);
      Serial.print("  BaroAlt: ");   Serial.println(r.baroAlt);
      Serial.print("  GPSAlt: ");    Serial.println(r.gpsAlt);
      Serial.print("  Lat: ");       Serial.println(r.lat,6);
      Serial.print("  Lon: ");       Serial.println(r.lon,6);
      Serial.print("  Pitch: ");     Serial.println(r.lora_pitch);
      Serial.print("  roll: ");      Serial.println(r.lora_roll);
      Serial.print("  Yaw: ");       Serial.println(r.lora_yaw);

      Serial.print("  Accel X : ");  Serial.print(r.accelX,3);
      Serial.print("  Accel Y : ");  Serial.print(r.accelY,3);
      Serial.print("  Accel Z : ");  Serial.println(r.accelZ,3);
    
      Serial.print("  Degree: ");     Serial.println(r.degree);
      Serial.print("  Speed: ");     Serial.println(r.speed);

      Serial.print("  Status: ");    Serial.println(r.status);
      Serial.print("  CRC: ");     Serial.println(r.crc);
      uint8_t calc_crc = calculateCRC8((uint8_t*)&r, sizeof(Rocket) - sizeof(r.crc));
      Serial.print("calc_crc = "); Serial.println(calc_crc);
      if (r.crc == calc_crc){
        Serial.println("  crc: Dogru");
      }
      else{
        Serial.println("  crc: Yanlis");
      }

    } 
    else {
      // Geçerli mesaj başlangıcı değil, at
      uint8_t trash;
      fifoPop(trash);
      Serial.print("///////////////////////////////////////////////////////////////////////////////////////////////////////// hatali byte: "); Serial.println(trash);
    }
  }
}




  void setup() {
  Serial.begin(115200);
  SerialAT.begin(9600, SERIAL_8N1, LORA_RX, LORA_TX);
  while (!Serial);
  delay(100);
  Serial.println("\n🚀 RX: waiting for Team1 packets…");
}

void loop() {
  if (SerialAT.available()) {
    uint8_t b = SerialAT.read();
    //Serial.print("RX: "); Serial.println(b);
    fifoPush(b);
  }

  parseFifo();
}