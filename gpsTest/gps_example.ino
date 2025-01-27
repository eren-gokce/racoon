
/* Davut BAY
  Youtube Kanalı
  NEO6M GPS TEST KODU
  02.11.2024
  */
#include <SoftwareSerial.h>
#include <TinyGPS++.h>

// RX ve TXpinlerimizi belirliyoruz
#define RX 10
#define TX 11


#define GPS_BAUD 9600

// gps isminde nesnemizi oluşturuyoruz başka bir isimde vere bilirsiniz
TinyGPSPlus gps;

// yazılımsal rx ve tx pinlerimizi oluşturuyoruz
SoftwareSerial gpsSerial(RX, TX);

void setup() {

  Serial.begin(9600);

  // gps seri haberleşmeyi başlatıyoruz
  gpsSerial.begin(GPS_BAUD);
  Serial.print("Konum bilgisi okunuyor ");
}

void loop() {

  unsigned long start = millis();

  while (millis() - start < 1000) {
    while (gpsSerial.available() > 0) {
      gps.encode(gpsSerial.read());
    }
      // İF İN İÇİNDE SIRASIYLA SERİ EKRANA BİLGİLERİ YAZDIRIYORUZ 
    if (gps.location.isUpdated()) {
      Serial.print("enlem: ");
      Serial.println(gps.location.lat(), 6);
      Serial.print("boylam: ");
      Serial.println(gps.location.lng(), 6);
      Serial.print("hız (km/h) = ");
      Serial.println(gps.speed.kmph());
      Serial.print("rakım (min)= ");
      Serial.println(gps.altitude.meters());
      Serial.print("HDOP = ");
      Serial.println(gps.hdop.value() / 100.0);
      Serial.print("uydu = ");
      Serial.println(gps.satellites.value());
      Serial.print("Time in UTC: ");
      //TOPLAMA ( + ) OPERETÖRÜ İLE İŞLEM YAPA  BİLMEK İÇİN ÖNCE ALDIGIMIZ DEĞERLERİ STRİNG E DÖNÜŞTÜRÜYORUZ. 
      Serial.println(String(gps.date.year()) + "/" + String(gps.date.month()) + "/" + String(gps.date.day()) + "--" + String((gps.time.hour()+3)) + ":" + String(gps.time.minute()) + ":" + String(gps.time.second()));
      Serial.println("");
    }
 
 }
}


/*
EN COK KULLANDIGIMIZ KOMUTLAR.
  RAWLAT KOMUTU İLE BELİRLİ BİR NOKTAYA OLAN UZAKLIGIMIZI BULABİLİYOR VE 
  UZAKLAŞIP YAKINLAŞTIGIMIZI TESPİT EDE BİLİYORUZ 

Serial.println(myGPS.location.lat(), 6);                    // Latitude in degrees (double)
Serial.println(myGPS.location.lng(), 6);                    // Longitude in degrees (double)
Serial.print(myGPS.location.rawLat().negative ? "-" : "+");
Serial.println(myGPS.location.rawLat().deg);                // Raw latitude in whole degrees
Serial.println(myGPS.location.rawLat().billionths);         // ... and billionths (u16/u32)
Serial.print(myGPS.location.rawLng().negative ? "-" : "+");
Serial.println(myGPS.location.rawLng().deg);                // Raw longitude in whole degrees
Serial.println(myGPS.location.rawLng().billionths);         // ... and billionths (u16/u32)
Serial.println(myGPS.date.value());                         // Raw date in DDMMYY format (u32)
Serial.println(myGPS.date.year());                          // Year (2000+) (u16)
Serial.println(myGPS.date.month());                         // Month (1-12) (u8)
Serial.println(myGPS.date.day());                           // Day (1-31) (u8)
Serial.println(myGPS.time.value());                         // Raw time in HHMMSSCC format (u32)
Serial.println(myGPS.time.hour());                          // Hour (0-23) (u8)
Serial.println(myGPS.time.minute());                        // Minute (0-59) (u8)
Serial.println(myGPS.time.second());                        // Second (0-59) (u8)
Serial.println(myGPS.time.centisecond());                   // 100ths of a second (0-99) (u8)
Serial.println(myGPS.speed.value());                        // Raw speed in 100ths of a knot (i32)
Serial.println(myGPS.speed.knots());                        // Speed in knots (double)
Serial.println(myGPS.speed.mph());                          // Speed in miles per hour (double)
Serial.println(myGPS.speed.mps());                          // Speed in meters per second (double)
Serial.println(myGPS.speed.kmph());                         // Speed in kilometers per hour (double)
Serial.println(myGPS.course.value());                       // Raw course in 100ths of a degree (i32)
Serial.println(myGPS.course.deg());                         // Course in degrees (double)
Serial.println(myGPS.altitude.value());                     // Raw altitude in centimeters (i32)
Serial.println(myGPS.altitude.meters());                    // Altitude in meters (double)
Serial.println(myGPS.altitude.miles());                     // Altitude in miles (double)
Serial.println(myGPS.altitude.kilometers());                // Altitude in kilometers (double)
Serial.println(myGPS.altitude.feet());                      // Altitude in feet (double)
Serial.println(myGPS.satellites.value());                   // Number of satellites in use (u32)
Serial.println(myGPS.hdop.value());                         // Horizontal Dim. of Precision (100ths-i32)

*/