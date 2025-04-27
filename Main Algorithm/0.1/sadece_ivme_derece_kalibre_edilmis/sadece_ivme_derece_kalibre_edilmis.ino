#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

Adafruit_MPU6050 mpu;

float axOffset = 0, ayOffset = 0, azOffset = 0;

void setup() {
  Serial.begin(115200);
  
  if (!mpu.begin()) {
    Serial.println("MPU6050 sensör bulunamadı!");
    while (1);
  }  
  
  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);

  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  int up_to = 1000;

  for (int i = 0; i < up_to; i++) {

    axOffset += a.acceleration.x;
    ayOffset += a.acceleration.y;
    azOffset += a.acceleration.z;

    delay(5);
  }

  axOffset /= up_to;
  ayOffset /= up_to;
  azOffset /= up_to;
}

void loop() {
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  float ax = a.acceleration.x - axOffset;
  float ay = a.acceleration.y - ayOffset;
  float az = a.acceleration.z - azOffset + 9.81;

  // İvmeölçer verileriyle yere göre derece hesaplama
  float deg = atan2(sqrt(ax * ax + ay * ay), az) * 180 / PI;

  // Sonuçları yazdır
  Serial.print("Deg: "); Serial.println(deg);
  
  delay(10); // 10 ms bekleme
}
