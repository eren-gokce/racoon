#include <Wire.h>
#include <Adafruit_BMP280.h>
#include "MPU6050_6Axis_MotionApps20.h"

//kalkis icin hiz verisi 5 m/s den buyuk

//leds
const uint8_t LEDa = 27;
const uint8_t LEDb = 26;
const uint8_t LEDc = 25;
const uint8_t LEDd = 33;
const uint8_t LEDe = 32;

const uint8_t LED[5] = { LEDa, LEDb, LEDc, LEDd, LEDe };

int x=1;
const int chipSelect = 53; // SD kart modülü CS pini

unsigned long previousMillis = 0;  
const long interval = 1000;  // 1 saniye

// Sensör nesneleri
Adafruit_BMP280 bmp;
MPU6050 mpu;

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
#define INTERRUPT_PIN 2  
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

/*void writeToSD(float altitude, float verticalSpeed, float verticalAccel, float yaw, float pitch, float roll){
    File dataFile = SD.open("sensor_data.txt", FILE_WRITE);
    
    if (dataFile) {
        dataFile.print(millis()); // Zaman damgası
        dataFile.print(",");
        dataFile.print("Yükseklik:");
        dataFile.print(m_n[0]);
        dataFile.print(",");
        dataFile.print("Dikey Hız:");
        dataFile.print(m_n[1]);
        dataFile.print(",");
        dataFile.print("Dikey İvme:");
        dataFile.print(m_n[2]);
        dataFile.print(",");
        dataFile.print("Yaw:");
        dataFile.print(ypr[0]);
        dataFile.print(",");
        dataFile.print("Pitch:");
        dataFile.print(ypr[1]);
        dataFile.print(",");
        dataFile.print("Roll:");
        dataFile.print(ypr[2]);
        dataFile.print(",");
        dataFile.println(); // Yeni satır eklenmeli

        dataFile.close();
        Serial.println("Veri SD karta yazıldı");
    } else {
        Serial.println("SD kart yazma hatası!");
    }
}*/

void roketinKalkisaGecmesi (){
  if(m_n[1] > 2.5 && m_n[0] >= 880){

    Serial.println("Roket kalkışa geçti");
    x++;
    //flaşakaydet
  }
}
void burnout(){
  if(m_n[0] > 3000)
  {
    x++;
  }
}

void apogee(){
  // if(m_n[1]<= 0.7){
  //   Serial.println("Roket apogee ulaştı");
  //   //fitile güç gidecek
  //   x++;
  //   digitalWrite(LED[3],HIGH);
  //   //flaşakaydet
      if((m_n[0]-830)> 3000)
  {
    
    x++;
  }
}

void parasut2 (){ // zeminden yuksekligin hesaplanmasi lazim
  if ((m_n[0] - 830) < 3000){
    Serial.println("Roket 2. paraşüt aşamasına geçti");
    digitalWrite(LED[3],HIGH);
     //fitile güç gidecek
   x++;
  //flaşakaydet
  }

}

void alcalmaKontrol(){
  if(m_n[1]==0 && m_n[2]==0){
    Serial.println("Roket yere iniş yaptı GPS verisi al");
    // gps verisi
  }
}

void setup() {
  Serial.begin(115200);
  Wire.begin();
  Wire.setClock(400000);

  for (uint8_t i = 0; i < 5; i++) {   // Tüm pinleri döngüyle ayarla
    pinMode(LED[i], OUTPUT);      // Çıkış yap
    digitalWrite(LED[i], LOW);    // Başlangıçta söndür
  }

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
  
//  if (!SD.begin(chipSelect)) {
//      Serial.println("SD kart başlatılamadı");
//  } else {
//      Serial.println("SD kart başlatıldı");
//  }
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

  // Kalman filtresi sonuçları: m_n[0] = güncellenmiş altimetre, m_n[1] = hız, m_n[2] = ivme
  Serial.print(" | Altitude: "); Serial.print(m_n[0] - 835);
  Serial.print(" | Speed: "); Serial.print(m_n[1]);
  Serial.print(" | Accel: "); Serial.println(m_n[2]);

  // MPU6050 açıları (Yaw, Pitch, Roll) seri yazdırma
  //Serial.print("Yaw: "); Serial.print(ypr[0], 4);
  //Serial.print(" | Pitch: "); Serial.print(ypr[1], 4);
  //Serial.print(" | Roll: "); Serial.println(ypr[2], 4);

  // Derece cinsinden açıları da yazdırma
  float yawDeg = ypr[0] * 180.0 / PI;
  float pitchDeg = ypr[1] * 180.0 / PI;
  float rollDeg = ypr[2] * 180.0 / PI;
  //Serial.print("Yaw (°): "); Serial.print(yawDeg, 2);
  //Serial.print(" | Pitch (°): "); Serial.print(pitchDeg, 2);
  //Serial.print(" | Roll (°): "); Serial.println(rollDeg, 2);

  delay(120);  // dt ~ 0.12 s

 unsigned long currentMillis = millis();
  
  // if (currentMillis - previousMillis >= interval) {
  //   previousMillis = currentMillis;
  //   writeToSD(m_n[0], m_n[1], m_n[2], ypr[0], ypr[1], ypr[2]);
  // }

  switch(x){
    case 0:
        roketinKalkisaGecmesi ();
      break;
      // burnout ekle importante
    case 7:
      burnout();

    case 1:
      apogee(); // eger hiz  100m/s den kucukse ve aci eksiye duserse parasut
      // aci sagklikliysa ve hiz eksiye duserse parashut
      break;

    case 2:
      parasut2 ();
      break;

    case 4:
      alcalmaKontrol();
      break;
    }

}