//functions.cpp
#include <Wire.h>
#include <Adafruit_BMP280.h>
#include "MPU6050_6Axis_MotionApps20.h"
#include "functions.h"
#include <LoRa_E32.h>
#include <SoftwareSerial.h>
#include <TinyGPS++.h>
#include <EEPROM.h> // flasha veri yazma

#pragma region global varaibles
//flag
volatile uint8_t flag = 254;
// m_n lerin değerleri
float yukseklikraw; //m_n[0]
float yukseklik; // m_n[0] - yukseklikraw
float hiz; // m_n[1]
float ivme; // m_n[2]
//ypr değerleri
float yaw;
float pitch;
float roll;
//ypr derece degerleri
float yawDeg;
float pitchDeg;
float rollDeg;
//avarage_calc icin millis
unsigned long millis_saved = 100;
unsigned long millis_counter;


#pragma endregion

float onceki_ortalama;
float son_ortalama;

void kalkis(){
  float avarage = avarage_in_given_time(1500 , hiz);
  if(avarage > 10){
    Serial.println("Roket kalkışa geçti");
    Serial.println("Roket kalkışa geçti");
    Serial.println("Roket kalkışa geçti");
    flag = 1;
    EEPROM.write(0, flag); //veriyi flaha yaz
    EEPROM.commit();  // veriyi kalicilastir
  }
}

void burnout(){
  float avarage = avarage_in_given_time(1500 , ivme);
  Serial.print("   onceki ortalama"); Serial.println(onceki_ortalama);
  if((onceki_ortalama - 3) < avarage && avarage < (onceki_ortalama + 3)){
    Serial.println("burnout gecti");
    Serial.println("burnout gecti");
    Serial.println("burnout gecti");
    Serial.print("   onceki_ortalama"); Serial.println(onceki_ortalama);
    flag = 2;
    EEPROM.write(0, flag); //veriyi flaha yaz
    EEPROM.commit();  // veriyi kalicilastir
  }
}

void apogee(){
  float avarage = avarage_in_given_time(1500 , hiz);
  if(10 <= avarage && avarage <= 30 && avarage != 0){ // duzenlencek default(-21 < x < 30)
    Serial.println("Roket apogee ulaştı");
    Serial.println("Roket apogee ulaştı");
    Serial.println("Roket apogee ulaştı");
    //++ fitile güç gidecek
    flag = 3;
    EEPROM.write(0, flag); //veriyi flaha yaz
    EEPROM.commit();  // veriyi kalicilastir
    digitalWrite(SURUKLENME_PARASUT_PIN, HIGH);
  }
}

uint8_t tekrar = 0;
void parasut_tekrarla(uint8_t x/* 0: Suruklenme parasutu, 1: Ana parasut */){
  switch(x){
    case 0:
      if(tekrar < 4){
        digitalWrite(SURUKLENME_PARASUT_PIN, LOW);
        delay(50);
        digitalWrite(SURUKLENME_PARASUT_PIN, HIGH);
        tekrar++;
      }
      else{
        tekrar = 0;
        flag = 4;
        EEPROM.write(0, flag); //veriyi flaha yaz
        EEPROM.commit();  // veriyi kalicilastir
      }
      break;
  
    case 1:
      digitalWrite(ANA_PARASUT_PIN, LOW);
      delay(300);
      digitalWrite(ANA_PARASUT_PIN, HIGH);
      break;
    default:
      break; 
  }
}

void parasut(){// zeminden yuksekligin hesaplanmasi lazim
  float avarage = avarage_in_given_time(1000 , yukseklik);
  if (70 < avarage && avarage <80 && avarage != 0){ // de[erler degisecek denemek icin yapildi default (400 < x < 600)
    Serial.println("Roket 2. paraşüt aşamasına geçti");
    Serial.println("Roket 2. paraşüt aşamasına geçti");
    Serial.println("Roket 2. paraşüt aşamasına geçti");
    flag = 5;
    EEPROM.write(0, flag); //veriyi flaha yaz
    EEPROM.commit();  // veriyi kalicilastir
    digitalWrite(ANA_PARASUT_PIN, HIGH);
  }

}

void alcalma(){
  float avarage = avarage_in_given_time(2000, yukseklik);
  if(-10 < avarage && avarage < 10 && avarage != 0){
    Serial.println("Roket yere iniş yaptı");
    flag = 6;
    EEPROM.write(0, flag); //veriyi flaha yaz
    EEPROM.commit();  // veriyi kalicilastir
  }
}

void write_to_sd(){
  //++ Burasi doldurulacak
}

float sum_avaragecalc = 0;
float avarage_in_given_ms;
unsigned int number_of_values_stored_in_sum = 1;

float avarage_in_given_time(unsigned long given_time /*as ms*/, float sensor_value){
  if (millis_counter < given_time)
  {
    sum_avaragecalc = sum_avaragecalc + sensor_value;
    number_of_values_stored_in_sum++;
    /* Serial.print("millis: "); Serial.println(millis());
    Serial.print("given_time: "); Serial.println(given_time);
    Serial.print("millis_saved: "); Serial.println(millis_saved);
    Serial.print("millis_counter: "); Serial.println(millis_counter);
    Serial.print("number of values stored in sum: "); Serial.println(number_of_values_stored_in_sum); */
    // Serial.print("sum: "); Serial.println(sum_avaragecalc);
    /* Serial.print(": "); Serial.print(); */
    return NAN;
  }
  else if(number_of_values_stored_in_sum == 0 || sum_avaragecalc == 0){
    millis_saved = millis();
  }
  else{
    if(number_of_values_stored_in_sum == 0){
      number_of_values_stored_in_sum = 1;
    }
    avarage_in_given_ms = sum_avaragecalc / number_of_values_stored_in_sum;
    /*       Serial.print("avarage: "); Serial.println(avarage_in_given_ms);
    Serial.print("given_time: "); Serial.println(given_time);
    Serial.print("millis_saved: "); Serial.println(millis_saved);
    Serial.print("millis_counter: "); Serial.println(millis_counter);
    Serial.print("number of values stored in sum: "); Serial.println(number_of_values_stored_in_sum);
    Serial.print("sum: "); Serial.println(sum_avaragecalc); */
    // Serial.print("sum: "); Serial.print(sum_avaragecalc);
    // Serial.print("avarage_in_given_ms: "); Serial.print(avarage_in_given_ms);
    sum_avaragecalc = 0;
    number_of_values_stored_in_sum = 0;
    millis_saved = millis();
    onceki_ortalama = son_ortalama;
    son_ortalama = avarage_in_given_ms;
    // Serial.print("   onceki_ortalama"); Serial.println(onceki_ortalama);
    return avarage_in_given_ms;
  }
}
