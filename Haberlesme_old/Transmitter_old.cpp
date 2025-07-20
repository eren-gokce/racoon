#include <Arduino.h>
#include <LoRa_E32.h>
#include <Wire.h>
#include <Adafruit_BMP280.h>
#include <MPU6050_light.h>
#include <SoftwareSerial.h>
#include <TinyGPS++.h>

#define LORA_RX     16
#define LORA_TX     17
#define GPS_RX_PIN  27
#define GPS_TX_PIN  26
#define GPS_BAUD    9600

HardwareSerial  SerialAT(2);
LoRa_E32        e32ttl(&SerialAT);
Adafruit_BMP280 bmp;
MPU6050         mpu(Wire);
SoftwareSerial  gpsSW(GPS_RX_PIN, GPS_TX_PIN);
TinyGPSPlus     gps;

static constexpr float G_CONST = 9.80665f;

#pragma pack(push,1)
struct Payload {
  uint8_t teamID;
  uint8_t packetCounter;
  float   baroAlt;
  float   gpsAlt;
  float   lat;
  float   lon;
  float   accelX;
  float   accelY;
  float   accelZ;
  uint8_t status;
};
#pragma pack(pop)

static uint8_t packetCounter = 0;

void setup() {
  Serial.begin(115200);
  SerialAT.begin(9600, SERIAL_8N1, LORA_RX, LORA_TX);
  gpsSW.begin(GPS_BAUD);
  delay(100);
  e32ttl.begin();
  Wire.begin(21,22);
  if (!bmp.begin(0x76)) { 
    Serial.println("BMP280 hatası!"); 
    while (1) delay(10); 
  }
  mpu.begin();
  Serial.println("\n🚀 TX başlıyor...");
}

void loop() {
  unsigned long t0 = millis();
  gpsSW.listen();

  static float lastGpsAlt = 0.0f;

  // 1 saniyeye kadar NMEA oku ve altitude güncelle
  while (millis() - t0 < 1000) {
    if (gpsSW.available() && gps.encode(gpsSW.read())) {
      if (gps.altitude.isUpdated()) {
        lastGpsAlt = gps.altitude.meters();
      }
    }
  }

  // sensörleri oku
  mpu.update();
  float baroAlt = bmp.readAltitude(1013.25F);
  float ax = mpu.getAccX() * G_CONST;
  float ay = mpu.getAccY() * G_CONST;
  float az = mpu.getAccZ() * G_CONST;

  // payload’u doldur
  Payload p;
  p.teamID  = 1;

  // <-- BURASI DEĞİŞTİ: packetCounter'ı her seferinde gönderilen byte kadar artırıyoruz
  packetCounter = packetCounter + sizeof(Payload);  
  p.packetCounter = packetCounter;                    

  p.baroAlt = baroAlt;
  p.gpsAlt  = lastGpsAlt;
  p.lat     = gps.location.isValid() ? gps.location.lat() : NAN;
  p.lon     = gps.location.isValid() ? gps.location.lng() : NAN;
  p.accelX  = ax;
  p.accelY  = ay;
  p.accelZ  = az;
  p.status  = 0;

  // ▶️▶️ Debug TX:
  Serial.print("TX bytes total: "); Serial.println(packetCounter);
  Serial.print("  BaroAlt: ");   Serial.println(p.baroAlt);
  Serial.print("  GPSAlt: ");    Serial.println(p.gpsAlt);
  Serial.print("  Lat: ");       Serial.println(p.lat,6);
  Serial.print("  Lon: ");       Serial.println(p.lon,6);
  Serial.print("  Accel : "); 
    Serial.print(p.accelX,3); Serial.print(", ");
    Serial.print(p.accelY,3); Serial.print(", ");
    Serial.println(p.accelZ,3);
  Serial.print("  Status: ");    Serial.println(p.status);
 
  // LoRa ile gönder
  ResponseStatus rs = e32ttl.sendFixedMessage(
    0, 40, 23,
    reinterpret_cast<uint8_t*>(&p),
    sizeof(p)
  );
  Serial.print("LoRa: "); Serial.println(rs.getResponseDescription());

  // Döngüyü 1 saniyeye tamamla
  while (millis() - t0 < 1000) delay(5);
}
