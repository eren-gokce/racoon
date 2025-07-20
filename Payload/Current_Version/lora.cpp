//lora.cpp
#include <Wire.h>
#include <Adafruit_BMP280.h>
#include "functions.h"
#include <LoRa_E32.h>
#include <SoftwareSerial.h>
#include <TinyGPS++.h>

#pragma region globals

float yukseklikraw;
float yukseklik;
float sicaklik;
float basinc;
float nem;

#pragma endregion

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

void call_lora(){
    lora_loop();
}

void lora_loop(){
  unsigned long t0 = millis();
  Payload p;

  gpsSW.listen();

  static float lastGpsAlt = 0.0f;

  while (millis() - t0 < 50) {
    if (gpsSW.available() && gps.encode(gpsSW.read())) {
      if (gps.altitude.isUpdated()) {
        lastGpsAlt = gps.altitude.meters();
      }
    }
  }

  // payload’u doldur
  p.id      = 131;
  p.baroAlt = yukseklik;
  p.gpsAlt  = lastGpsAlt;
  p.lat     = gps.location.isValid() ? gps.location.lat() : NAN;
  p.lon     = gps.location.isValid() ? gps.location.lng() : NAN;
  p.pressure= basinc;
  p.temp    = sicaklik;
  p.humidity= 0; // nem sensörü elde edince düzenlencek
  p.crc     = calculateCRC8((uint8_t*)&p, sizeof(Payload) - sizeof(p.crc));

  // ▶️▶️ Debug TX:
  Serial.print("  Id: "); Serial.println(p.id);
  Serial.print("  BaroAlt: ");   Serial.println(p.baroAlt);
  Serial.print("  GPSAlt: ");    Serial.println(p.gpsAlt);
  Serial.print("  Lat: ");       Serial.println(p.lat,6);
  Serial.print("  Lon: ");       Serial.println(p.lon,6);
  Serial.print("  Pressure: ");       Serial.println(p.pressure);
  Serial.print("  Temp: ");       Serial.println(p.temp);
  Serial.print("  Humidity: ");       Serial.println(p.humidity);

  Serial.print("  CRC: ");       Serial.println(p.crc);

  // LoRa ile gönder
  ResponseStatus rs = e32ttl.sendFixedMessage(
    0, 40, 23,
    reinterpret_cast<uint8_t*>(&p),
    sizeof(p)
  );
  Serial.print("LoRa: "); Serial.println(rs.getResponseDescription());
}