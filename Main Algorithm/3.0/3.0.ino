//main.ino
#include <Wire.h>
#include <Adafruit_BMP280.h>
#include "MPU6050_6Axis_MotionApps20.h"
#include "functions.h"
#include <LoRa_E32.h>
#include <SoftwareSerial.h>
#include <TinyGPS++.h>

#define INTERRUPT_PIN 2 

//diger nesneler
HardwareSerial  SerialAT(2);
LoRa_E32        e32ttl(&SerialAT);
SoftwareSerial  gpsSW(GPS_RX_PIN, GPS_TX_PIN);
TinyGPSPlus     gps;

// Sensör nesneleri
Adafruit_BMP280 bmp;
MPU6050 mpu;

const int FlagButtonPin = 14; //*******************************************sadece flag atlama butonu
volatile bool ButtonFlag = false; //************************************sadece flag atlama butonu

void IRAM_ATTR handleInterrupt() { //****************************************************sadece flag atlama butonu
  ButtonFlag = true;
}

#pragma region timer starts timer after 10s and yukseklik
#include <Ticker.h>
void first_flag_function(){
  flag = 0;
  yukseklikraw = yukseklik;
  Serial.println("Kalibrasyon Tamamlandı");
}
Ticker first_flag_timer;
#pragma endregion


#pragma region Kalman Filter

// Zaman adımı: dt, Python tarafından hesaplanan ortalama değere göre (0.12 s)
float dt = 0.12;  

// Kalman filtre için durum vektörü: [altitude, vertical velocity, vertical acceleration]
// Burada vertical acceleration, kontrol girişi olarak kullanılacaktır.
float m_n[3] = {0.0, 0.0, 0.0};  // Başlangıç durumu (yükseklik, dikey hız,dikey ivme)
float P_n[3][3] = { {1, 0, 0}, {0, 1, 0}, {0, 0, 1} };

// Sabit ivmeli model için sistem dinamik matrisi F:
float F[3][3] = {
  {1, dt, 0.5 * dt * dt},
  {0, 1, dt},
  {0, 0, 1}
};

// Python optimizasyonundan elde edilen Q (süreç gürültüsü kovaryansı):
float Q[3][3] = {
  {0.1, 0,   0},
  {0,   0.1, 0},
  {0,   0,   0.1}
};

// Ölçüm gürültüsü: Ölçümde yalnızca altimetre kullanıldığından, R bir skaler olarak 0.1 alınır.
float R_meas = 0.1;  

// Ölçüm matrisi: yalnızca altimetre ölçümü kullanılır, H_new = [1, 0, 0]
float H_new[1][3] = { {1, 0, 0} };

// MPU6050 DMP için
 
volatile bool MPUInterrupt = false;
bool DMPReady = false;
uint8_t devStatus;
uint16_t packetSize;
uint8_t FIFOBuffer[64];

Quaternion q;
VectorFloat gravity;
float ypr[3];  // Yaw, Pitch, Roll

// MPU6050 DMP Interrupt Handler
void DMPDataReady() {
  MPUInterrupt = true;
}

// Yardımcı: 3x3 matris çarpımı (A * B, sonuç 3x3)
void mat3Mul(float A[3][3], float B[3][3], float C[3][3]) {
  for (int i = 0; i < 3; i++){
    for (int j = 0; j < 3; j++){
      C[i][j] = 0;
      for (int k = 0; k < 3; k++){
        C[i][j] += A[i][k] * B[k][j];
      }
    }
  }
}

// Yardımcı: 3x3 matrisin transpozu ile çarpımı (A * F^T, F^T[i][j]=F[j][i])
void mat3MulFTrans(float A[3][3], float F_mat[3][3], float C[3][3]) {
  for (int i = 0; i < 3; i++){
    for (int j = 0; j < 3; j++){
      C[i][j] = 0;
      for (int k = 0; k < 3; k++){
        C[i][j] += A[i][k] * F_mat[j][k];
      }
    }
  }
}

/*
  Modified Kalman Filter:
  - Prediction aşamasında kontrol girişi olarak MPU6050’den okunan vertical ivme (accel_meas) kullanılır.
  - Durum güncellemesi, yalnızca altimetre (BMP280) ölçümü kullanılarak inovasyon hesaplanır.
  
  Prediction:
    x_pred[0] = m_n[0] + m_n[1]*dt + 0.5*dt^2 * accel_meas;
    x_pred[1] = m_n[1] + dt * accel_meas;
    x_pred[2] = accel_meas;    // Ölçülen ivme direkt atanır.
    
  Measurement update:
    y = alt_meas - x_pred[0];
    S = P_pred[0][0] + R_meas;
    K = P_pred(:,0) / S;
    x_updated = x_pred + K * y;
*/

void kalmanFilter_modified(float alt_meas, float accel_meas) {
  // 1. Tahmin Aşaması
  float x_pred[3];
  x_pred[0] = m_n[0] + m_n[1] * dt + 0.5 * dt * dt * accel_meas;
  x_pred[1] = m_n[1] + dt * accel_meas;
  x_pred[2] = accel_meas;
  
  // P_pred = F * P_n * F^T + Q
  float temp[3][3];
  float P_pred[3][3];
  mat3Mul(F, P_n, temp);
  mat3MulFTrans(temp, F, P_pred);
  for (int i = 0; i < 3; i++){
    for (int j = 0; j < 3; j++){
      P_pred[i][j] += Q[i][j];
    }
  }
  
  // 2. Ölçüm İnovasyonu: Sadece altimetre ölçümü kullanılır
  float y = alt_meas - x_pred[0];
  
  // 3. İnovasyon Kovaryansı: S = P_pred[0][0] + R_meas
  float S = P_pred[0][0] + R_meas;
  if (S == 0) return;
  
  // 4. Kalman Kazancı: K = P_pred(:,0) / S
  float K[3];
  K[0] = P_pred[0][0] / S;
  K[1] = P_pred[1][0] / S;
  K[2] = P_pred[2][0] / S;
  
  // 5. Durum Güncellemesi
  float x_updated[3];
  x_updated[0] = x_pred[0] + K[0] * y;
  x_updated[1] = x_pred[1] + K[1] * y;
  x_updated[2] = x_pred[2] + K[2] * y;
  
  // 6. Kovaryans Güncellemesi: P = (I - K * H_new) * P_pred, H_new = [1, 0, 0]
  float I_KH[3][3];
  for (int i = 0; i < 3; i++){
    for (int j = 0; j < 3; j++){
      float KH = (j == 0) ? K[i] : 0;
      I_KH[i][j] = (i == j ? 1.0 : 0.0) - KH;
    }
  }
  float P_updated[3][3] = { {0,0,0}, {0,0,0}, {0,0,0} };
  for (int i = 0; i < 3; i++){
    for (int j = 0; j < 3; j++){
      for (int k = 0; k < 3; k++){
        P_updated[i][j] += I_KH[i][k] * P_pred[k][j];
      }
    }
  }
  
  // Global durum ve kovaryans güncellemesi
  for (int i = 0; i < 3; i++){
    m_n[i] = x_updated[i]; //(dikey hız )
    for (int j = 0; j < 3; j++){
      P_n[i][j] = P_updated[i][j];
    }
  }
}

#pragma endregion

void setup() {
  Serial.begin(115200);
  SerialAT.begin(9600, SERIAL_8N1, LORA_RX, LORA_TX); //lora ve gps
  Wire.begin();
  Wire.setClock(400000);

  gpsSW.begin(GPS_BAUD); //lora
  e32ttl.begin(); // lora

  // Optimize edilmiş Q ve R parametrelerini ekrana yazdırıyoruz:
  Serial.println("Optimized Kalman Filter Parameters:");
  Serial.println("Q:");
  Serial.println("0.1  0    0");
  Serial.println("0    0.1  0");
  Serial.println("0    0    0.1");
  Serial.println("R:");
  Serial.println("0.1");
  
  // BMP280 Başlatma (I2C adresi 0x76 veya 0x77 olabilir)
  if (!bmp.begin(0x76)) {
    Serial.println("BMP280 sensor not detected.");
    while (1);
  }

  Serial.println("Initializing MPU6050...");
  mpu.initialize();
  pinMode(INTERRUPT_PIN, INPUT);
  Serial.println(mpu.testConnection() ? "MPU6050 connection successful" : "MPU6050 connection failed");
  
  devStatus = mpu.dmpInitialize();
  if (devStatus == 0) {
    mpu.CalibrateAccel(6);
    mpu.CalibrateGyro(6);
    mpu.PrintActiveOffsets();
    
    mpu.setDMPEnabled(true);
    attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN), DMPDataReady, RISING);
    Serial.println("DMP ready!");
    DMPReady = true;
    packetSize = mpu.dmpGetFIFOPacketSize();
  } else {
    Serial.print("DMP Initialization failed (code ");
    Serial.print(devStatus);
    Serial.println(")");
  }

  first_flag_timer.once(6.0/*second*/, first_flag_function); //timer starts after 10s

  pinMode(FlagButtonPin, INPUT_PULLDOWN);  // Buton için dahili pull-up *********************************************sadece flag atlama butonu
  attachInterrupt(digitalPinToInterrupt(FlagButtonPin), handleInterrupt, FALLING); //*****************************sadece flag atlama butonu
}

void loop() {
  if (!DMPReady) return;

  // MPU6050'dan verileri oku (sadece vertical ivme için)
  if (mpu.dmpGetCurrentFIFOPacket(FIFOBuffer)) { 
    mpu.dmpGetQuaternion(&q, FIFOBuffer);
    mpu.dmpGetGravity(&gravity, &q);
    mpu.dmpGetYawPitchRoll(ypr, &q, &gravity); // Bu satır eklendi, böylece ypr güncellenir.
  }
  
  // BMP280'dan altimetre ölçümü (deniz seviyesi basıncı 1013.25 hPa)
  float altitude = bmp.readAltitude(1013.25);
  // MPU6050'dan okunan vertical ivme: gravity.z (kontrol girişi olarak kullanılacak)
  float accel_z = gravity.z;  

  // Kalman filtresi uygulaması: altimetre ölçümü, kontrol girişi olarak accel_z
  kalmanFilter_modified(altitude, accel_z);

  //m_n[] lerin normal adlara esitlenmesi
  yukseklik = m_n[0] - yukseklikraw;
  hiz = m_n[1];
  ivme = m_n[2];

  // Kalman filtresi sonuçları: m_n[0] = güncellenmiş altimetre, m_n[1] = hız, m_n[2] = ivme
  Serial.print(" | Altituderaw: "); Serial.print(yukseklikraw);
  Serial.print(" | Altitude: "); Serial.print(yukseklik);
  Serial.print(" | Speed: "); Serial.print(hiz);
  Serial.print(" | Accel: "); Serial.print(ivme);

  Serial.print(" | Current Flag: "); Serial.println(flag);


  yaw = ypr[0];
  pitch = ypr[1];
  row = ypr[2];

  // MPU6050 açıları (Yaw, Pitch, Roll) seri yazdırma
  //Serial.print("Yaw: "); Serial.print(ypr[0], 4);
  //Serial.print(" | Pitch: "); Serial.print(ypr[1], 4);
  //Serial.print(" | Roll: "); Serial.println(ypr[2], 4);

  // Derece cinsinden açıları da yazdırma
  yawDeg = ypr[0] * 180.0 / PI;
  pitchDeg = ypr[1] * 180.0 / PI;
  rollDeg = ypr[2] * 180.0 / PI;
  //Serial.print("Yaw (°): "); Serial.print(yawDeg, 2);
  //Serial.print(" | Pitch (°): "); Serial.print(pitchDeg, 2);
  //Serial.print(" | Roll (°): "); Serial.println(rollDeg, 2);

  delay(120);  // dt ~ 0.12 s //degistirildi 120 idi

  millis_counter = millis() - millis_saved;

  if (ButtonFlag) {//*****************************************sadece flag atlama butonu
    ButtonFlag = false;             // Bayrağı sıfırla
    Serial.println("Flag atlandi");       // Kesmede değil, burada yazdır
    flag++;
  }

  lora_loop();

  switch(flag){
    case 0:
      kalkis();
      break;
    
    case 1:
      burnout();
      break;

    case 2:
      apogee();
      break;

    case 3:
      parasut();
      break;

    case 4:
      alcalma();
      break;

    //case 5:
      //gps verileri
  }
}