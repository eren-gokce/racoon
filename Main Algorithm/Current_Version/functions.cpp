#include <Wire.h>
#include <Adafruit_BMP280.h>
#include "MPU6050_6Axis_MotionApps20.h"
#include "functions.h"

// ana algoritmadaki varaible ları functions dosyasına taşır
extern float yukseklik;
extern float hiz;
extern float ivme;
extern int x;
extern const uint8_t LEDa = 27;
extern const uint8_t LEDb = 26;
extern const uint8_t LEDc = 25;
extern const uint8_t LEDd = 33;
extern const uint8_t LEDe = 32;
extern const uint8_t LED[5] = { LEDa, LEDb, LEDc, LEDd, LEDe };


void kalkis(){
  if(hiz > 10){
    Serial.println("Roket kalkışa geçti");
    x = 1;
    //++ flaşa kaydet
  }
}

void apogee(){
  if(-21 >= hiz && hiz <= 30){
    Serial.println("Roket apogee ulaştı");
    //++ fitile güç gidecek
    x = 2;
    digitalWrite(LED[3],HIGH);
    //++ flaşa kaydet
  }
}

void parasut(){ // zeminden yuksekligin hesaplanmasi lazim
  if (400 < yukseklik && yukseklik <600){
    Serial.println("Roket 2. paraşüt aşamasına geçti");
     //++ fitile güç gidecek
   x = 3;
  //++ flaşa kaydet
  }

}

void alcalma(){
  if((hiz <= 1 && -1 >= hiz) && (ivme <= 1 && -1 >= ivme)){
    Serial.println("Roket yere iniş yaptı GPS verisi al");
    x = 4;
    //++ gps verisi
  }
}