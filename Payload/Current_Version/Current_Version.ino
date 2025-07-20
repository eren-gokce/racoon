//main.ino
#include <Wire.h>
#include <Adafruit_BMP280.h>
#include "functions.h"
#include <LoRa_E32.h>
#include <SoftwareSerial.h>
#include <TinyGPS++.h>
#include <EEPROM.h> // flasha veri yazma
#include "esp_task_wdt.h" // Task watchdog timer

#include <Ticker.h>
float altitude;
void first_flag_function(){
  yukseklikraw = altitude;
}
Ticker t;

unsigned long lora_timer;

// diger nesneler
HardwareSerial  SerialAT(2);
LoRa_E32        e32ttl(&SerialAT);
SoftwareSerial  gpsSW(GPS_RX_PIN, GPS_TX_PIN);
TinyGPSPlus     gps;

// Sensör nesneleri
Adafruit_BMP280 bmp;

void setup() {
  Serial.begin(115200);
  SerialAT.begin(9600, SERIAL_8N1, LORA_RX, LORA_TX); //lora ve gps
  Wire.begin();
  Wire.setClock(400000);

  gpsSW.begin(GPS_BAUD); //lora
  e32ttl.begin(); // lora
  
  // BMP280 Başlatma (I2C adresi 0x76 veya 0x77 olabilir)
  if (!bmp.begin(0x76)) {
    Serial.println("BMP280 sensor not detected.");
    while (1);
  }

  esp_task_wdt_config_t twdt_config = { // task watchdog configuraiton
    .timeout_ms = 3000, // 3 saniye içinde resetlenmeli
    .idle_core_mask = 0, // idle task izlenmiyor
    .trigger_panic = true
  };

  esp_task_wdt_init(&twdt_config); // Watchdog’u başlat
  esp_task_wdt_add(NULL);          // Mevcut görev (loop task) TWDT’ye abone edildi
  
  lora_timer = millis();

  t.once(2.0/*second*/, first_flag_function);
}

void loop() {
  // BMP280'dan altimetre ölçümü (deniz seviyesi basıncı 1013.25 hPa)
  altitude = bmp.readAltitude(1013.25);
  yukseklik = altitude - yukseklikraw;

  basinc = bmp.readPressure();
  sicaklik = bmp.readTemperature();

  call_lora();  // Sadece tetik alındığında veri gönder

  // if(millis() - lora_timer >= 0){
  //   call_lora();
  //   lora_timer = millis();
  // }

  esp_task_wdt_reset(); // watchdog'u besle
  
}