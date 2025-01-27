#include <SoftwareSerial.h>

SoftwareSerial gpsSerial(10, 11); // RX = 4, TX = 3 denedim olmadi 10 11 verdim oldu.

void setup() {
  Serial.begin(9600);         // PC ile seri haberleşme
  gpsSerial.begin(9600);      // GPS modülü ile haberleşme
  Serial.println("GPS Verileri Gönderiliyor...");
}

void loop() {
  if (gpsSerial.available()) {
    char c = gpsSerial.read();
    Serial.print(c);           // Bilgisayara gönder
  }
}
