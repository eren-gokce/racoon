#include <Arduino.h>
#include <LoRa_E32.h>
#include <cstring>  // memcpy
#include "driver/uart.h"

HardwareSerial SerialAT(2);
#define LORA_RX 16
#define LORA_TX 17

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
  if (fifoAvailable() >= sizeof(Payload)) {
    uint8_t startByte;
    startByte = fifoBuffer[fifoTail];
    // fifoPop(startByte);

    if (startByte == 131 && fifoAvailable() >= sizeof(Payload)) {
      // Payload mesajı
      Payload p;
      for (int i = 0; i < sizeof(Payload); i++) {
        fifoPop(((uint8_t*)&p)[i]);  // baştan itibaren oku
      }

      uint8_t calc_crc = calculateCRC8((uint8_t*)&p, sizeof(Payload) - sizeof(p.crc));
      if(calc_crc == p.crc){
        //Serial.print("  id: ");        Serial.println(p.id);
        Serial.print("  BaroAlt: ");   Serial.println(p.baroAlt);
        Serial.print("  GPSAlt: ");    Serial.println(p.gpsAlt);
        Serial.print("  Lat: ");       Serial.println(p.lat,6);
        Serial.print("  Lon: ");       Serial.println(p.lon,6);
        Serial.print("  pressure: ");       Serial.println(p.pressure);
        Serial.print("  temp: ");       Serial.println(p.temp);
        Serial.print("  humidity: ");       Serial.println(p.humidity);
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
  Serial.println("\n🚀 RX: waiting for packets…");
}

void loop() {
  if (SerialAT.available()) {
    uint8_t b = SerialAT.read();
    //Serial.print("RX: "); Serial.println(b);
    fifoPush(b);
  }

  parseFifo();
}