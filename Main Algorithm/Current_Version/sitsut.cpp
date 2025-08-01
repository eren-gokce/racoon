#include <Wire.h>
#include <Adafruit_BMP280.h>
#include "MPU6050_6Axis_MotionApps20.h"
#include "functions.h"


float accel_x;
float accel_y;
float accel_z;

float checksum = 0; //degistirilcek

#pragma region fifo
#define BUFFER_SIZE 256
uint8_t fifoBuffer[BUFFER_SIZE];
uint8_t fifoHead = 0;
uint8_t fifoTail = 0;

void fifoPush(uint8_t b) { // fifo ya byte ekleme
  fifoBuffer[fifoHead] = b;
  fifoHead = (fifoHead + 1) % BUFFER_SIZE;

  // FIFO dolarsa, en eski veriyi ez
  if (fifoHead == fifoTail) {
    fifoTail = (fifoTail + 1) % BUFFER_SIZE;
  }
}

bool fifoPop(uint8_t &b) { // fifo byte okuma
  if (fifoHead == fifoTail) return false; // boş
  b = fifoBuffer[fifoTail];
  fifoTail = (fifoTail + 1) % BUFFER_SIZE;
  return true;
}

uint8_t fifoAvailable() {  // fifo veri uzunlugu
  return (fifoHead + BUFFER_SIZE - fifoTail) % BUFFER_SIZE;
}
#pragma endregion

uint8_t calculateChecksum(const uint8_t* data, size_t length) {
  uint16_t sum = 0;
  for (size_t i = 0; i < length; i++) {
    sum += data[i];
  }
  return (uint8_t)(0x100 - (sum & 0xFF));  // Toplam + checksum ≡ 0 mod 256
}

void parseFifo() {
  if (fifoAvailable() >= 36) {
    uint8_t startByte;
    startByte = fifoBuffer[fifoTail];

    if (startByte == 171 && fifoAvailable() >= sizeof(Sut)) {
      Sut s;
      for (int i = 0; i < sizeof(Sut); i++) {
        fifoPop(((uint8_t*)&s)[i]);  // baştan itibaren oku
      }
      Serial.print("  irtifa: ");   Serial.println(s.irtifa);
      Serial.print("  basinc: ");   Serial.println(s.basinc);
      Serial.print("  ivmex: ");    Serial.println(s.ivmex);
      Serial.print("  ivmey: ");    Serial.println(s.ivmey);
      Serial.print("  ivmez: ");    Serial.println(s.ivmez);
      Serial.print("  acix: ");     Serial.println(s.acix);
      Serial.print("  aciy: ");     Serial.println(s.aciy);
      Serial.print("  aciz: ");     Serial.println(s.aciz);
      Serial.print("  checksum: "); Serial.println(s.checksum);
      uint8_t calc_checksum = calculateChecksum((uint8_t*)&s, sizeof(Sut) - 3);
      Serial.print("calc_checksum = "); Serial.println(calc_checksum);
      if (s.checksum == calc_checksum){
        Serial.println("  checksum: Dogru");
      }
      else{
        Serial.println("  checksum: Yanlis");
      }

    }
    else {
      // Geçerli mesaj başlangıcı değil, at
      uint8_t trash;
      fifoPop(trash);
      Serial.print("///////////////////////////////////////////////////////////////////////////////////////////////////////// hatali byte: "); Serial.println(trash);
    }
  }
}

uint8_t sit_paket[36];

float ikiBasamak(float deger) {
  return round(deger * 100.0f) / 100.0f;
}

uint8_t byte_array[4] = {0};

void donustur(float *deger) {
	byte_donustur a;
	a.sayi = *deger;
	byte_array[0] = a.array[0];
	byte_array[1] = a.array[1];
	byte_array[2] = a.array[2];
	byte_array[3] = a.array[3];
}

void sit(){
  // Serial.println("sit hesapliyor");
  sit_paket[0] = 171;  //0xAB

  float irtifa = ikiBasamak(bmp.readAltitude(1013.25));
  donustur(&irtifa);
  sit_paket[1] = byte_array[0];
  sit_paket[2] = byte_array[1];
  sit_paket[3] = byte_array[2];
  sit_paket[4] = byte_array[3];

  float basinc = ikiBasamak(bmp.readPressure());
  donustur(&basinc);
  sit_paket[5] = byte_array[0];
  sit_paket[6] = byte_array[1];
  sit_paket[7] = byte_array[2];
  sit_paket[8] = byte_array[3];

  float ivmeX = ikiBasamak(accel_x);
  donustur(&ivmeX);
  sit_paket[9] = byte_array[0];
  sit_paket[10] = byte_array[1];
  sit_paket[11] = byte_array[2];
  sit_paket[12] = byte_array[3];

  float ivmeY = ikiBasamak(accel_y);
  donustur(&ivmeY);
  sit_paket[13] = byte_array[0];
  sit_paket[14] = byte_array[1];
  sit_paket[15] = byte_array[2];
  sit_paket[16] = byte_array[3];

  float ivmeZ = ikiBasamak(ivme);
  donustur(&ivmeZ);
  sit_paket[17] = byte_array[0];
  sit_paket[18] = byte_array[1];
  sit_paket[19] = byte_array[2];
  sit_paket[20] = byte_array[3];

  float aciX = ikiBasamak(pitch);
  donustur(&aciX);
  sit_paket[21] = byte_array[0];
  sit_paket[22] = byte_array[1];
  sit_paket[23] = byte_array[2];
  sit_paket[24] = byte_array[3];

  float aciY = ikiBasamak(roll);
  donustur(&aciY);
  sit_paket[25] = byte_array[0];
  sit_paket[26] = byte_array[1];
  sit_paket[27] = byte_array[2];
  sit_paket[28] = byte_array[3];

  float aciZ = ikiBasamak(yaw);
  donustur(&aciZ);
  sit_paket[29] = byte_array[0];
  sit_paket[30] = byte_array[1];
  sit_paket[31] = byte_array[2];
  sit_paket[32] = byte_array[3];
  

  sit_paket[33] = calculateChecksum((uint8_t*)sit_paket, 33);
  sit_paket[34] = 0x0D;
  sit_paket[35] = 0x0A;

  // for(int i = 0; i < 36; i++){
  //   Serial.println(sit_paket[i]);
  // }

  Serial.write(sit_paket, 36);
  delay(60);
} 

void sut(){
  if (Serial.available()) {
    uint8_t b = SerialAT.read();
    Serial.print("byte: "); Serial.println(b);
    fifoPush(b);
  }
  parseFifo();
}