//functions.h
#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <Wire.h>
#include <Adafruit_BMP280.h>
#include "functions.h"
#include <LoRa_E32.h>
#include <SoftwareSerial.h>
#include <TinyGPS++.h>

#pragma region lora

void call_lora();
//void lora_counter();
uint8_t calculateCRC8(const uint8_t* data, size_t len);

extern HardwareSerial  SerialAT;
extern LoRa_E32        e32ttl;
extern SoftwareSerial  gpsSW;
extern TinyGPSPlus     gps;

#define LORA_RX     16
#define LORA_TX     17
#define GPS_RX_PIN  27
#define GPS_TX_PIN  26
#define GPS_BAUD    9600

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
#pragma endregion

void lora_loop();

#pragma region externs

extern float yukseklikraw;
extern float yukseklik;
extern float sicaklik;
extern float basinc;
extern float nem;

#pragma endregion

#endif