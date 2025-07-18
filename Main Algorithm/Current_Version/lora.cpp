//lora.cpp
#include <Wire.h>
#include <Adafruit_BMP280.h>
#include "MPU6050_6Axis_MotionApps20.h"
#include "functions.h"
#include <LoRa_E32.h>
#include <SoftwareSerial.h>
#include <TinyGPS++.h>

static uint8_t packetCounter = 0;

// unsigned long lora_millis_saved = 0;
// unsigned long lora_time_passed;

// void lora_counter(){
//   Payload p;
//   lora_time_passed = millis() - lora_millis_saved;
//   if(lora_time_passed >= 400){
//     //send lora
//     ResponseStatus rs = e32ttl.sendFixedMessage(
//       0, 40, 23,
//       reinterpret_cast<uint8_t*>(&p),
//       sizeof(p)
//     );
//     Serial.print("LoRa: "); Serial.println(rs.getResponseDescription());

//     lora_millis_saved = millis();
//   }
// }

void call_lora(){
  if(flag != 254){
    lora_loop();
  }
}

void lora_loop(){
  unsigned long t0 = millis();
  Payload p;
  //lora_time_passed = millis() - lora_millis_saved;
  //if(lora_time_passed >= 400){

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
  p.teamID  = 1;

  // <-- BURASI DEĞİŞTİ: packetCounter'ı her seferinde gönderilen byte kadar artırıyoruz
  packetCounter = packetCounter + sizeof(Payload);  
  p.packetCounter = packetCounter;                    

  p.baroAlt = yukseklik;
  p.gpsAlt  = lastGpsAlt;
  p.lat     = gps.location.isValid() ? gps.location.lat() : NAN;
  p.lon     = gps.location.isValid() ? gps.location.lng() : NAN;
  p.lora_yaw = yaw;
  p.lora_pitch = pitch;
  p.lora_row = row;
  p.accelX  = 0;
  p.accelY  = ivme;
  p.accelZ  = 0;
  p.speed = hiz;
  p.status  = 0;

  // ▶️▶️ Debug TX:
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

  // LoRa ile gönder
  ResponseStatus rs = e32ttl.sendFixedMessage(
    0, 40, 23,
    reinterpret_cast<uint8_t*>(&p),
    sizeof(p)
  );
  Serial.print("LoRa: "); Serial.println(rs.getResponseDescription());
}