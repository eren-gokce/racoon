/*----------------------------------------------------------
 *  ROCKET Kalman + Apogee LED   (ESP32 DevKit v1, Polling)
 *  – Sensörler : MPU6050 (DMP)  +  BMP280
 *  – Dikey eksen = Z    (kart “düz” monte)
 *  – Launch‑Arming:     v > LAUNCH_SPEED
 *  – Apogee:            hız +→− geçiş  &&  irtifa düşüşü > APOGEE_ALT_DROP
 *  – LED (GPIO 25) HIGH  : Apogee’de
 *    LED LOW             : Yer irtifası (h < GROUND_ALT) geçildiğinde
 *  Besma & ChatGPT – 22 Apr 2025
 *---------------------------------------------------------*/
#include <Wire.h>
#include <Adafruit_BMP280.h>
#include "MPU6050_6Axis_MotionApps20.h"

/* ---------------- Donanım Seçimleri -------------------- */
constexpr uint8_t PIN_LED_APOG   = 25;     // LED / MOSFET
constexpr uint8_t BMP280_ADDR    = 0x76;   // 0x76 veya 0x77
constexpr float   SEA_LEVEL_HPA  = 1013.25f;

/* --- DMP ivme ölçeği (+‑2 g modu için) ----------------- */
#define ACCEL_SCALE 16384.0f               // 1 g = 16384 LSB

/* ---------------- Uçuş Eşikleri ------------------------ */
const float LAUNCH_SPEED      = 3.0f;      // m/s  – fırlatma algı eşiği
const float APOGEE_ALT_DROP   = 2.0f;      // m    – apogeede h azalması
const float GROUND_ALT        = 5.0f;      // m    – LED söndürme eşiği

/* ---------------- Nesneler ----------------------------- */
MPU6050 mpu;
Adafruit_BMP280 bmp;

/* ---------------- Zaman Adımı -------------------------- */
const float dt = 0.12f;                    // ≈ 120 ms (typ. DMP FIFO)

/* ---------------- Kalman Değişkenleri ------------------ */
float x_k[3] = {0,0,0};                    // [h, v, a]
float P_k[3][3] = {{1,0,0},{0,1,0},{0,0,1}};
const float F[3][3] = {{1,dt,0.5f*dt*dt},
                       {0,1,dt},
                       {0,0,1}};
const float Q[3][3] = {{0.05,0,0},
                       {0,0.05,0},
                       {0,0,0.05}};
const float R_meas  = 0.2f;

/* ----------- Yardımcı 3×3 Çarpım Fonksiyonları -------- */
void matMul3(const float A[3][3], const float B[3][3], float C[3][3]){
  for(int i=0;i<3;i++) for(int j=0;j<3;j++){
    C[i][j]=0;
    for(int k=0;k<3;k++) C[i][j]+=A[i][k]*B[k][j];
  }
}
void matMul3_FT(const float A[3][3], const float Fm[3][3], float C[3][3]){
  for(int i=0;i<3;i++) for(int j=0;j<3;j++){
    C[i][j]=0;
    for(int k=0;k<3;k++) C[i][j]+=A[i][k]*Fm[j][k]; // F^T
  }
}
void kalmanUpdate(float alt_meas, float accel_z){
  float xp[3] = { x_k[0] + x_k[1]*dt + 0.5f*dt*dt*accel_z,
                  x_k[1] + accel_z*dt,
                  accel_z };

  float tmp[3][3], Pp[3][3];
  matMul3(F, P_k, tmp);
  matMul3_FT(tmp, F, Pp);
  for(int i=0;i<3;i++) for(int j=0;j<3;j++) Pp[i][j]+=Q[i][j];

  float y = alt_meas - xp[0];
  float S = Pp[0][0] + R_meas;
  float K[3] = {Pp[0][0]/S, Pp[1][0]/S, Pp[2][0]/S};

  for(int i=0;i<3;i++) x_k[i] = xp[i] + K[i]*y;

  float I_KH[3][3];
  for(int i=0;i<3;i++) for(int j=0;j<3;j++){
    float KH = (j==0) ? K[i] : 0;
    I_KH[i][j] = (i==j) - KH;
  }
  float Pn[3][3]={{0}};
  for(int i=0;i<3;i++) for(int j=0;j<3;j++)
    for(int k=0;k<3;k++) Pn[i][j]+=I_KH[i][k]*Pp[k][j];
  for(int i=0;i<3;i++) for(int j=0;j<3;j++) P_k[i][j]=Pn[i][j];
}

/* ---------------- Uçuş Durum Bayrakları --------------- */
bool  launchArmed = false;
bool  apogee      = false;
float vPrev       = 0;
float prevAlt     = 0;

/* ==================== SETUP =========================== */
void setup(){
  Serial.begin(115200);
  Wire.begin(21, 22, 400000);               // SDA, SCL, 400 kHz

  pinMode(PIN_LED_APOG, OUTPUT);  digitalWrite(PIN_LED_APOG, LOW);

  if(!bmp.begin(BMP280_ADDR)){ Serial.println("BMP280 yok!"); while(1); }

  mpu.initialize();
  if(!mpu.testConnection()){ Serial.println("MPU6050 yok!"); while(1); }
  if(mpu.dmpInitialize()!=0){ Serial.println("DMP hata!"); while(1); }
  mpu.CalibrateAccel(6);
  mpu.CalibrateGyro(6);
  mpu.setDMPEnabled(true);

  Serial.println("Sistem hazır – Kalman + Apogee LED");
}

/* ==================== LOOP ============================ */
void loop(){
  /* 1) FIFO taşma koruması */
  if (mpu.getFIFOCount() == 1024) { mpu.resetFIFO(); return; }
  if (!mpu.dmpPacketAvailable())   return;

  /* 2) Paket oku */
  uint8_t fifoBuf[64];
  if(!mpu.dmpGetCurrentFIFOPacket(fifoBuf)) return;

  /* 3) Lineer ivme (Z ekseni) */
  Quaternion q; VectorFloat gravity;
  VectorInt16 aaRaw, aaReal;

  mpu.dmpGetQuaternion(&q, fifoBuf);
  mpu.dmpGetGravity(&gravity, &q);
  mpu.dmpGetAccel(&aaRaw, fifoBuf);
  mpu.dmpGetLinearAccel(&aaReal, &aaRaw, &gravity);

  float accel_z  = aaReal.z / ACCEL_SCALE;          // +yukarı
  float altitude = bmp.readAltitude(SEA_LEVEL_HPA);

  /* 4) Kalman */
  kalmanUpdate(altitude, accel_z);

  /* 5) Launch & Apogee mantığı */
  if(!launchArmed && x_k[1] > LAUNCH_SPEED){
      launchArmed = true;
      Serial.println("LAUNCH detected – system armed");
  }

  if(launchArmed && !apogee &&
     vPrev > 0 && x_k[1] <= 0 &&                 // hız +→−
     (prevAlt - x_k[0]) > APOGEE_ALT_DROP)       // irtifa azaldı
  {
      apogee = true;
      digitalWrite(PIN_LED_APOG, HIGH);
      Serial.println("APOGEE – LED ON");
  }

  if(apogee && x_k[0] < GROUND_ALT){             // yere yakın
      digitalWrite(PIN_LED_APOG, LOW);
  }

  /* 6) Değişken güncelle */
  vPrev   = x_k[1];
  prevAlt = x_k[0];

  /* 7) Telemetri */
  Serial.printf("h=%.2f m  v=%.2f m/s  a=%.2f g  armed=%d  apg=%d\n",
                x_k[0], x_k[1], x_k[2], launchArmed, apogee);
}