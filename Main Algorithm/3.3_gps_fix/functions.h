//functions.h
#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <Wire.h>
#include <Adafruit_BMP280.h>
#include "MPU6050_6Axis_MotionApps20.h"
#include "functions.h"
#include <LoRa_E32.h>
#include <SoftwareSerial.h>
#include <TinyGPS++.h>

#define SURUKLENME_PARASUT_PIN 32
#define ANA_PARASUT_PIN 33


#pragma region lora

void call_lora();
//void lora_counter();

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
#pragma endregion

void kalkis();
void burnout();
void apogee();
void parasut_tekrarla(uint8_t x);
void parasut();
void alcalma();

void write_to_sd();

float avarage_in_given_time(unsigned long given_time /*as ms*/, float sensor_value);

void lora_loop();

#pragma region externs
//flag
extern volatile uint8_t flag;
// m_n lerin değerleri
extern float yukseklikraw; //m_n[0]
extern float yukseklik; // m_n[0] - yukseklikraw
extern float hiz; // m_n[1]
extern float ivme; // m_n[2]
//ypr değerleri
extern float yaw;
extern float pitch;
extern float row;
//ypr derece degerleri
extern float yawDeg;
extern float pitchDeg;
extern float rollDeg;
//avarage_calc icin millis
extern unsigned long millis_saved;
extern unsigned long millis_counter;

#pragma endregion

#endif