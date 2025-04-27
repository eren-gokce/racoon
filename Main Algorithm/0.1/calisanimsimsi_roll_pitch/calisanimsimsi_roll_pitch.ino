#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

Adafruit_MPU6050 mpu;

// Filtreleme için zaman ve sabitler
float alpha = 0.98; // Complementary Filter sabiti
float dt;           // Zaman farkı
unsigned long prevTime;

void setup() {
  Serial.begin(115200);
  
  if (!mpu.begin()) {
    Serial.println("MPU6050 sensör bulunamadı!");
    while (1);
  }  
  
  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);

  prevTime = millis();
}

void loop() {
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  // Zaman farkı
  unsigned long currentTime = millis();
  dt = (currentTime - prevTime) / 1000.0; // milisaniyeyi saniyeye çevir
  prevTime = currentTime;

  // İvmeölçer verileriyle roll ve pitch hesaplama
  float accRoll = atan2(a.acceleration.y, a.acceleration.z) * 180 / PI;
  float accPitch = atan2(-a.acceleration.x, sqrt(a.acceleration.y * a.acceleration.y + a.acceleration.z * a.acceleration.z)) * 180 / PI;

  // Jiroskop verileriyle roll ve pitch hesaplama
  static float gyroRoll = 0, gyroPitch = 0;
  gyroRoll += g.gyro.x * dt;
  gyroPitch += g.gyro.y * dt;

  // Complementary Filter ile birleştirme
  static float roll = 0, pitch = 0;
  roll = alpha * (roll + g.gyro.x * dt) + (1 - alpha) * accRoll;
  pitch = alpha * (pitch + g.gyro.y * dt) + (1 - alpha) * accPitch;

  // Sonuçları yazdır
  Serial.print("Roll: "); Serial.print(roll);
  Serial.print(" Pitch: "); Serial.print(pitch);
  Serial.print(" Yaw (Gyro): "); Serial.println(g.gyro.z * dt); // Gyro ile kaba tahmin
  
  delay(10); // 10 ms bekleme
}
