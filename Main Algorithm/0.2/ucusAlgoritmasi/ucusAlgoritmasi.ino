#include <Adafruit_MPU6050.h>
#include <Adafruit_BMP280.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include <SPI.h>

float Q = 0.001; // Process noise covariance
float R = 0.03;  // Measurement noise covariance
float P = 1;     // Estimation error covariance
float K;         // Kalman gain
float filteredX = 0;
float filteredY = 0;
float filteredZ = 0;
float axOffset = 0, ayOffset = 0, azOffset = 0;
float gxOffset = 0, gyOffset = 0, gzOffset = 0;
float baseAltitude = 0; // Kalibrasyon sirasinda elde edilen referans yukseklik
float h = bmp.readAltitude(1030.9); // Basınca göre hesaplanan yükseklik
float a = sqrt(pow(axOffset, 2) + pow(ayOffset, 2) + pow(azOffset, 2)); // 3 eksende ivme büyüklüğü

float applyKalmanFilter(float measurement, float &estimate) {
    P += Q; // Prediction step
    K = P / (P + R); // Kalman gain
    estimate = estimate + K * (measurement - estimate); // Update estimate
    P *= (1 - K); // Update error covariance
    return estimate;
}

void sensorKalibrasyonu() {
    Serial.println("Sensör kalibrasyonu başlatılıyor");
    sensors_event_t a, g, temp;
    int calibrationLoops = 1000;

    for (int i = 0; i < calibrationLoops; i++) {
        mpu.getEvent(&a, &g, &temp);
        axOffset += a.acceleration.x;
        ayOffset += a.acceleration.y;
        azOffset += a.acceleration.z;
        gxOffset += g.gyro.x;
        gyOffset += g.gyro.y;
        gzOffset += g.gyro.z;
        delay(5);
    }

    axOffset /= calibrationLoops;
    ayOffset /= calibrationLoops;
    azOffset /= calibrationLoops;
    gxOffset /= calibrationLoops;
    gyOffset /= calibrationLoops;
    gzOffset /= calibrationLoops;

    baseAltitude = bmp.readAltitude(1030.9); // Referans yükseklik alınması
    Serial.println("Kalibrasyon tamamlandı.");
}


void MPU() {// MPU ÇALIŞMASININ KONTROLÜ İÇİN 
for(i=0;i<3;i++){ // kontrol için 
  if (!mpu.begin()) {// başlamdıysa
    Serial.println("Failed to find MPU6050 chip");
    MPU() // çalışmadığı için başa aldım
    }
    else {
      delay(10); //çalıştı kontrole gidecek
    }
    
  }
  Serial.println(" MPU çalışmaya hazır");
}
 
  
void BMP(){ // ÇALIŞMA KONTROLÜ İÇİN
  int i;
   while ( !Serial ) delay(100);   // wait for native usb
  Serial.println(F("BMP280 test"));
  unsigned status;
  //status = bmp.begin(BMP280_ADDRESS_ALT, BMP280_CHIPID);
  status = bmp.begin(0x76);
  for (i=0;i<3;i++){
  if (!status) {
    Serial.println(F("Could not find a valid BMP280 sensor, check wiring or "
                      "try a different address!"));
    Serial.print("SensorID was: 0x"); Serial.println(bmp.sensorID(),16);
    Serial.print("        ID of 0xFF probably means a bad address, a BMP 180 or BMP 085\n");
    Serial.print("   ID of 0x56-0x58 represents a BMP 280,\n");
    Serial.print("        ID of 0x60 represents a BME 280.\n");
    Serial.print("        ID of 0x61 represents a BME 680.\n");

    while (1) delay(10);
  }
  }

 

}
void GPS (){
}

void  lora(){
}

void ucusKontrol(){

for (int i=0; i<3; i++){

  if (h>kalibrasyondan alınan yükseklik && a> kalibrasyondan alınan ivme){
    
    delay(500);
  }
  else {
    delay(10000);
    ucusKontrol();
  }

  Serial.println(" ucus saglandi");
}
}

void apogee(){

  for(int i=0; i<3; i++){

      if(pitch<0 && dikey hız<=0 && h1<h2){
        delay(500);
      }
      else{
        delay(500);
        apogee();
      }
Serial.prinln("apogee ulaştın");
  }
}

void parasut2(){

  for( int i=0; i<3;i++){

  if( 400<h && h<600){
    delay(500);
  }
  else{
    delay(500);
    parasut2();
  }
Serial.println("2.parasut acılabilir");
  }
}

void alcalmaKontrol(){

  for(int i=0 ; i<3; i++){
      if(h1==h2 && dikey hız==0 && a==0){
        delay(500);
      }
      else {
        delay(500);
        alcalmaKontrol();
      }
      Serial.println(" roket yerde gps bul");

  }
}

void setup(void) {
    Serial.begin(115200);
    Serial.println("kalibrasyona basla");
  MPU();
  BMP();
  GPS();
  lora();
  sensorKalibrasyonu();
  Serial.println("kalibrasyon tamam");
  Serial.println("sisteme kaydet");
    while (!Serial)
        delay(10); // will pause Zero, Leonardo, etc until serial console opens

    mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
    Serial.print("Accelerometer range set to: ");
    switch (mpu.getAccelerometerRange()) {
    case MPU6050_RANGE_2_G:
        Serial.println("+-2G");
        break;
    case MPU6050_RANGE_4_G:
        Serial.println("+-4G");
        break;
    case MPU6050_RANGE_8_G:
        Serial.println("+-8G");
        break;
    case MPU6050_RANGE_16_G:
        Serial.println("+-16G");
        break;
    }

    mpu.setGyroRange(MPU6050_RANGE_500_DEG);
    Serial.print("Gyro range set to: ");
    switch (mpu.getGyroRange()) {
    case MPU6050_RANGE_250_DEG:
        Serial.println("+- 250 deg/s");
        break;
    case MPU6050_RANGE_500_DEG:
        Serial.println("+- 500 deg/s");
        break;
    case MPU6050_RANGE_1000_DEG:
        Serial.println("+- 1000 deg/s");
        break;
    case MPU6050_RANGE_2000_DEG:
        Serial.println("+- 2000 deg/s");
        break;
    }

    mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
    Serial.print("Filter bandwidth set to: ");
    switch (mpu.getFilterBandwidth()) {
    case MPU6050_BAND_260_HZ:
        Serial.println("260 Hz");
        break;
    case MPU6050_BAND_184_HZ:
        Serial.println("184 Hz");
        break;
    case MPU6050_BAND_94_HZ:
        Serial.println("94 Hz");
        break;
    case MPU6050_BAND_44_HZ:
        Serial.println("44 Hz");
        break;
    case MPU6050_BAND_21_HZ:
        Serial.println("21 Hz");
        break;
    case MPU6050_BAND_10_HZ:
        Serial.println("10 Hz");
        break;
    case MPU6050_BAND_5_HZ:
        Serial.println("5 Hz");
        break;
    }

    sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp);

    int up_to = 1000;

    // BMP280 initialization
    while (!Serial) delay(100); // wait for native USB
    Serial.println(F("BMP280 test"));
    unsigned status;
    status = bmp.begin(0x76);
    if (!status) {
        Serial.println(F("Could not find a valid BMP280 sensor, check wiring or "
                        "try a different address!"));
        while (1) delay(10);
    }

    bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,     /* Operating Mode. */
                    Adafruit_BMP280::SAMPLING_X2,     /* Temp. oversampling */
                    Adafruit_BMP280::SAMPLING_X16,    /* Pressure oversampling */
                    Adafruit_BMP280::FILTER_X16,      /* Filtering. */
                    Adafruit_BMP280::STANDBY_MS_500); /* Standby time. */

    Serial.println("");
    delay(100);


// GPS VE LORA EKLENECEK



}


void loop() {
    sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp);    // Apply Kalman filter to acceleration data
    h = bmp.readAltitude(1030.9); // Basınca göre hesaplanan yükseklik
    a = sqrt(pow(axOffset, 2) + pow(ayOffset, 2) + pow(azOffset, 2)); // 3 eksende ivme büyüklüğü
    float accX = applyKalmanFilter(a.acceleration.x - axOffset, filteredX);
    float accY = applyKalmanFilter(a.acceleration.y - ayOffset, filteredY);
    float accZ = applyKalmanFilter(a.acceleration.z - azOffset + 10.1, filteredZ);

    /* Print out the filtered values */
    Serial.print("Filtered Acceleration X: ");
    Serial.print(accX);
    Serial.print(", Y: ");
    Serial.print(accY);
    Serial.print(", Z: ");
    Serial.print(accZ);
    Serial.println(" m/s^2");

    Serial.print("Rotation X: ");
    Serial.print(g.gyro.x - gxOffset);
    Serial.print(", Y: ");
    Serial.print(g.gyro.y - gyOffset);
    Serial.print(", Z: ");
    Serial.print(g.gyro.z - gzOffset);
    Serial.println(" rad/s");

    Serial.print(F("Approx altitude = "));
    Serial.print(bmp.readAltitude(1030.9)); /* Adjusted to local forecast! */
    Serial.println(" m");

    Serial.println();
    delay(200);

    // GPS VE LORA EKLENECEK 

ucusKontrol();
// eeprom kaydetme işi 
apogee();
// eeprom kaydetme işi 
// 1. paraşüt ateşleme
// görev yükünü ayır
parasut2();
// eeprom kaydetme işi 
alcalmaKontrol();
// gps verisinin bilgisayara kaydolması
Serial.println ("Tebrikler ekip roketi bulmaya gidin");

}


}