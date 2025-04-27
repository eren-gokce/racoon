#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

Adafruit_MPU6050 mpu;

// Filtreleme için zaman ve sabitler
float alpha = 0.98; // Complementary Filter sabiti
float dt;           // Zaman farkı
unsigned long prevTime;

float axOffset = 0, ayOffset = 0, azOffset = 0;
float gxOffset = 0, gyOffset = 0, gzOffset = 0;

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
    gxOffset += g.gyro.x;
    gyOffset += g.gyro.y;
    gzOffset += g.gyro.z;

    delay(5);
  }

  axOffset /= up_to;
  ayOffset /= up_to;
  azOffset /= up_to;
  gxOffset /= up_to;
  gyOffset /= up_to;
  gzOffset /= up_to;

  prevTime = millis();
}

void loop() {
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  float ax = a.acceleration.x - axOffset;
  float ay = a.acceleration.y - ayOffset;
  float az = a.acceleration.z - azOffset + 9.81;
  float gx = g.gyro.x - gxOffset;
  float gy = g.gyro.y - gyOffset;
  float gz = g.gyro.z - gzOffset;

  // Zaman farkı
  unsigned long currentTime = millis();
  dt = (currentTime - prevTime) / 1000.0; // milisaniyeyi saniyeye çevir
  prevTime = currentTime;

  // İvmeölçer verileriyle roll ve pitch hesaplama
  float accRoll = atan2(ay, az) * 180 / PI;
  float accPitch = atan2(-ax, sqrt(ay * ay + az * az)) * 180 / PI;

  // Jiroskop verileriyle roll ve pitch hesaplama
  static float gyroRoll = 0, gyroPitch = 0, gyroYaw = 0;
  gyroRoll += gx * dt;
  gyroPitch += gy * dt;
  gyroYaw += gz * dt;

  // Complementary Filter ile birleştirme
  static float roll = 0, pitch = 0;
  roll = alpha * (roll + gx * dt) + (1 - alpha) * accRoll;
  pitch = alpha * (pitch + gy * dt) + (1 - alpha) * accPitch;

  // Sonuçları yazdır
  Serial.print("Roll: "); Serial.print(roll);
  Serial.print(" Pitch: "); Serial.print(pitch);
  Serial.print(" Yaw (Gyro): "); Serial.println(gyroYaw*50); // Gyro ile kaba tahmin
  
  delay(10); // 10 ms bekleme
}
