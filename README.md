# Roket Aviyonik 🚀

### Güncel :

<details> <summary> Watchdog </summary>

Interrupt Watchdog (IWDT)
RTC Watchdog Timer (RTC_WDT)

ESP32 mimarisinde iki önemli sistem watchdog’u olan Interrupt Watchdog (IWDT) ve RTC Watchdog Timer (RTC_WDT), 
uygulama koduna herhangi bir müdahaleye gerek kalmadan sistem seviyesinde otomatik olarak çalışmaktadır.
IWDT, FreeRTOS işletim sistemi tarafından her tick’te beslenerek, ISR’ların veya kritik bölümlerin sistemin kilitlenmesine neden olup olmadığını denetler. 
RTC_WDT ise, sistem açıldığında (boot aşamasında) devreye girerek, main() fonksiyonuna zamanında ulaşılamazsa otomatik reset işlemi gerçekleştirir. 
Bu iki watchdog, ESP32 sisteminin kararlı çalışmasını sağlamakta, olası kilitlenmelerde sistemin kendi kendine toparlanmasına imkân tanımaktadır.

Task Watchdog Timer (TWDT)

ESP32 platformunda kullanılan Task Watchdog Timer (TWDT), belirli görevlerin (task'ların) belirli bir süre içerisinde düzgün şekilde 
çalışıp çalışmadığını denetlemek amacıyla kullanılan bir yazılımsal koruma mekanizmasıdır. 
Kullanıcı tarafından başlatılarak aktif hâle getirilen TWDT’ye, izlenmesi istenen görevler abone edilir. 
Bu görevler, belirlenen zaman aralığında esp_task_wdt_reset() fonksiyonu ile watchdog'u “beslemeli”; aksi takdirde 
TWDT bu görevin kilitlendiğini varsayarak sistemin hata mesajı üretmesini (panic) ya da resetlenmesini sağlar.
Güvenlik-kritik uygulamalarda TWDT, sistemin ana görevlerinin donması, veri akışının kesilmesi veya 
infinite loop gibi yazılım hatalarının oluşması durumunda müdahale ederek sistemin kendi kendini kurtarabilmesini sağlar.

```cpp
#include "esp_task_wdt.h"

void setup() {
  // ... mevcut kurulumlar ...

  esp_task_wdt_config_t twdt_config = {
    .timeout_ms = 3000, // 3 saniye içinde resetlenmeli
    .idle_core_mask = 0, // idle task izlenmiyor
    .trigger_panic = true
  };

  esp_task_wdt_init(&twdt_config); // Watchdog’u başlat
  esp_task_wdt_add(NULL);          // Mevcut görev (loop task) TWDT’ye abone edildi
}

void loop() {
  // Uçuş kontrol fonksiyonlarını çağır
  kalkis();
  burnout();
  apogee();
  parasut();
  alcalma();
  call_lora();

  esp_task_wdt_reset(); // 🔁 watchdog'u besle

  delay(200);
}
```
</details>

## [Yapılcaklar notu](https://ostimteknikuniversitesi-my.sharepoint.com/:w:/g/personal/230201060_ostimteknik_edu_tr/EbsuAKLU0ShNoTfdPXoBlwgBV6bBdwqXOMXw21tmuLnfXQ?e=zlj0Fm)


<details> <summary> old  </summary>

![güncel Algo flowchart](https://github.com/user-attachments/assets/4ee9dc47-58e5-4329-be02-5780bf1bafeb)
![güncel Algo flowchart](https://github.com/user-attachments/assets/764aee20-59d5-423e-8e08-6983022160b2)
	
- [Eski Arayüz Reposu](https://github.com/CerenSultanCETIN/RocketInterface13.git)

<div style="display: flex; justify-content: center; gap: 20px;">
  <a href="https://github.com/user-attachments/assets/b3c92df3-6dc9-4abe-9a28-f702d46e05a6">
    <img src="https://github.com/user-attachments/assets/b3c92df3-6dc9-4abe-9a28-f702d46e05a6" alt="resim1" width="750" />
  </a>
<div style="display: flex; justify-content: center; gap: 20px;">
  <a href="https://github.com/user-attachments/assets/648fd35b-3c85-447b-8fb1-8ce616d4887b">
    <img src="https://github.com/user-attachments/assets/648fd35b-3c85-447b-8fb1-8ce616d4887b" alt="resim1" width="750" />
  </a>

### 2025.04.27 KTR şablonundan olan görevlerim : 

- Özgün UKB’lerin algoritmaları açık ve net bir şekilde açıklanmalıdır.
 Algoritma açıklanırken karmaşık ve uzun cümleler kullanılmamalı,
 akış şemaları kullanılmalıdır. 
Kullanılan algoritmaya ilişkin akış şemasında; 
algoritmada kullanılan parametrelerin hangi sensörden geldiği açık ve net bir şekilde anlatılmalıdır.
 Kurtarma sistemini tetikleyecek parametreler listelenmeli ve neden seçildiği belirtilmelidir (kullandığı parametreleri doğru sensörlerden almayan veya eksik olarak açıklayan takımlar diskalifiye edilecektir). 
Algoritma içerisinde haberleşme ve yer istasyonu ile ilgili adımlar var ise bunlar paylaşılmamalıdır.
 (besma // görselleştirerek yap)

- Özgün UKB’lerin test edilmesi kapsamında test cihazı ile Ö-UKB arasında çalışacak olan yazılımın UKB ana algoritması ile nasıl entegre edildiği ve çalışma yapısı açık şekilde algoritma şeması olarak paylaşılmalıdır. (semih/enes/besma)

### 2025.04.26
Deney videosu (Semih) 
https://drive.google.com/drive/folders/1odC9xmdg6W0WZIL8I4gqv5UqY6kFndTy?usp=sharing

### 2025.04.24 Mete başkana algoritma hakkında soru soruldu ve bunun çıktısı şöyle oldu : 

🌟 Özet Strateji:

🧠 Sensör Füzyonlama (Sensor Fusion) + 📉 Kalman Filtresi

Bu iki yöntem, farklı sensörlerden gelen verileri birleştirerek daha doğru ve güvenilir ölçümler sağlar.

⸻

💡 Elimizdeki Üç Faktör:
	1.	İvme (Accelerometer)
	•	Başlangıç ve motor bitimi (burn-out) sırasında yüksek titreşim nedeniyle barometre bozulabilir.
	•	Bunu kontrol etmek için ivmeölçer (accelerometer) kullanılır.
	•	Kalman filtresi ile filtrelenerek anlık ivme patlamalarına karşı önlem alınır.
	2.	Dikey Hız (Vertical Speed)
	•	Barometre verisinden veya ivme verisinin integralinden hesaplanabilir.
	•	Özellikle motor kapanışından sonra roketin apogee noktasına ulaşıp ulaşmadığı burada anlaşılır.
	3.	Yükseklik (Altitude)
	•	Barometrik sensör (BMP280 gibi) ile ölçülür ama burn-out ve ani basınç değişimlerinde güvenilirliği düşer.
	•	Bu nedenle ivme ve dikey hız verisiyle çapraz kontrol yapılır.

⸻

🔍 Neden Kalman Filtresi?

Kalman filtresi, sistemdeki gürültülü sensör verilerini birleştirerek en iyi tahmini yapar. Yani:
	•	Barometre + ivmeölçer verisi → Yükseklik ve dikey hızın en güvenilir hali elde edilir.
	•	Başlangıçta barometredeki hataları ivme ile check edersiniz.
	•	Hata varsa Kalman filtresi bunu “güvenilir olmayan veri” olarak değerlendirip tahminini düzeltir.

⸻

🔄 Sensör Füzyonlama
	•	Sensör füzyonlama, farklı kaynaklardan gelen verileri birleştirerek tek bir anlamlı veri seti üretir.
	•	Örneğin:
	•	BMP280 (yükseklik)
	•	MPU6050 (ivme ve gyro)
	•	Kalman filtresi ile bu sensörlerin birleşimi yapılır.

⸻

📌 Uygulamada Ne Yapılmalı?
	1.	MPU6050 → ivme ve gyro verisini oku.
	2.	BMP280 → basınca dayalı yükseklik verisini al.
	3.	Tüm bu verileri Kalman filtresi ile işleyerek:
	•	Gerçek yükseklik
	•	Dikey hız
	•	Apogee tespiti
gibi önemli bilgileri güvenilir şekilde elde et.

---

📌 KTR Raporunda Algoritma’dan Ne Bekleniyor?

- 📄 “Özgün UKB’lerin algoritmaları açık ve net bir şekilde açıklanmalıdır.”
- 📄 “Algoritma açıklanırken karmaşık ve uzun cümleler kullanılmamalı, akış şemaları kullanılmalıdır.”
- 📄 “Algoritmada kullanılan parametrelerin hangi sensörden geldiği açık ve net bir şekilde anlatılmalıdır.”
- 📄 “Kurtarma sistemini tetikleyecek parametreler listelenmeli ve neden seçildiği belirtilmelidir.”
- 📄 “Algoritma içerisinde haberleşme ve yer istasyonu ile ilgili adımlar var ise bunlar paylaşılmamalıdır.”




### 2025.04.20 / Algoritma Deney Testi / Asansör Kullanıldı hızın değişimi gözlemlendi. ( Erenle birlikte yaptık )
##### KTR de bu kısımda bizden istenilen şu şekilde yazılmış: 

- Algoritma Testleri:
Bu test ile takimlar özgün UKB'leri ile aym sensör mimarisine sahip bir prototip olusturmalidir. Prototip için devre kartinin üretilmis olmasi gerekmemektedir. Sensörler modül olarak breadboard üzerinde bir araya getirilebilir. Devre kartini üretmis olan takimlar nihai devre kartlari ile bu testi gerçeklestirebilirler.
Testin amaci ayrilma sistemini aktiflestirecek olan algoritmanin çalisma prensibinin kontrolüdür. Takimlar gerekirse algoritmalari için belirledikleri esik degerleri degistirerek ancak algoritma mantigini degistirmeden algoritmalarin1 test etmelidirler. Yapilan test sonucunda ayrilma sisteminin aktiflestirildigi görsel veya isitsel bir çikti ile gösterilmelidir.
Ornek bir test senaryosu olarak, ayrilma algoritmasinda roketin yer normali ile yaptigi açi ve irtifa degerini kullanan bir sistem için breadboard üzerine yerlestirilen bir prototip aviyonik sistemin yer ile yaptigi açi el ile degistirilebilir.
Sonrasinda ise sistem bir emis sistemi (örnegin elektrik süpürgesi) yardimiyla vakumlama yapip irtifa degisimi saglanabilir. Yapilan degisiklikler sonucun kurtarma sisteminin aktivasyonunu modellemek için bir LED lambas1 yakilabilir ya da bir ses sistemi (örnegin buzzer) aktive edilebilir. Yapilan bu adimlar en fazla 30 saniyelik bir video ile gösterilmelidir.


  https://github.com/user-attachments/assets/4e7f96be-68da-4bf2-be7a-c1ad1da02b06

  - BUNUN SESLENDİRİLMESİ YAPILMASI GEREKİYOR ASANSÖRDE ÇIKILDIĞINI VE ŞU AMAÇLA YAPILMASI GEREKTİĞİNİ SÖYLEYEN BİR MONTAJ 

<a href="https://github.com/user-attachments/assets/b571d8d4-8fc2-40d7-b4c7-0fd44b211a0b">
  <img src="https://github.com/user-attachments/assets/b571d8d4-8fc2-40d7-b4c7-0fd44b211a0b" alt="" width="450" />
</a>
<a href="https://github.com/user-attachments/assets/4e7f96be-68da-4bf2-be7a-c1ad1da02b06">
  <img src="https://github.com/user-attachments/assets/4e7f96be-68da-4bf2-be7a-c1ad1da02b06" alt="" width="450" />
</a>

### 💥 2025.03.15 / KTR Raporlarının hazırlanması 

#### Questions:

1. Kodlanış şekilleri, i2c mi SPI mı UART mı ?
2. Hazır kütüphane var mı yoksa driver mı yazılacak ?

<details> <summary> Alternatif Modül</summary> 
2025_01_25/27

GPS için araştırma sürecine girdim ancak araştırırken birkez daha aldığım GPS modülunu denemeye karar verdim, başka bir kodla EKTEDİR, Cerendeydi GPS getirdi bende test için eve getirdim. Pazartesi günü Yusuf ve Şenol hocalarla konuşmak için gideceğim Ömer de eşlik edecek inşallah o güne kadar detaylı bir GPS modül araştırma raporlamasını yapmış olur test etmiş olurum yeni kodu umarım lock olur. 

- [NEO-6M GPS MODÜL ile konum, hız, irtifa, tarih bilgisi alma | ESP VE ARDUİNO | TİNYGPS++ LİBRARY](https://www.youtube.com/watch?v=yDfn9ZEc5MA)
- [Videodaki verilen kod](https://drive.google.com/drive/folders/1Ig9ccjL5o0V9VCOxEw0sJ4Tpg6B3VTsB)
- [TinyGPS++ Library](https://github.com/mikalhart/TinyGPSPlus?tab=readme-ov-file)

--
{Elimizdeki GY-NEO6MV2 GPS Modülü}
Roket takımları genelde Adafruit Ultimate GPS Modülü veya elimizdeki GY-NEO6MV2 GPS Modülünü kullanmış 
benim beğendiğim  u-blox NEO-M8N-0 
---
Alternatif Modüller :
-----------------------------

Adafruit Ultimate GPS 

[ Adafruit Ultimate GPS Breakout - 66 channel w/10 Hz updates: Bu modül, yüksek hassasiyetli GPS verileri sağlar ve 10 Hz'e kadar güncelleme hızı sunar. Ayrıca, dahili veri kaydedici ve antene sahiptir.
](https://www.adafruit.com/product/5440)
----------------------------------------------------- --------
[ Adafruit Ultimate GPS Breakout with GLONASS + GPS - PA1616D: Bu model, hem GPS hem de GLONASS uydu sistemlerini destekler, bu sayede daha hızlı ve hassas konum belirleme imkanı sunar.](https://www.adafruit.com/product/746?utm_source=chatgpt.com)

Ublox GYGPSV1 NEO-8M GPS Modülü:
1. [Ublox GY-GPSV3 NEO-8M M8N GPS Modülü](https://www.robocombo.com/ublox-gy-gpsv3-neo-8m-m8n-gps-modulu--3196) 472,56 ₺


2. [u-blox NEO-M8N-0](https://www.mouser.com.tr/ProductDetail/u-blox/NEO-M8N-0?qs=zW32dvEIR3unZhZI0KRbew%3D%3D
)[DataSheet](https://www.mouser.com.tr/datasheet/2/1025/NEO_M8_FW3_DataSheet_UBX_15031086-3180589.pdf)


Beitian BN-220 GPS Modülü
1. [AddictiveRC](https://shop.addictiverc.com/products/beitian-bn-220-gps-module-3-6v-5-5v-ttl-level-dual-gnss-module-built-in-led-flash-for-rc-fpv-drone) $20

2. [AMAZON](https://www.amazon.com.tr/Rordigo-HMC5883-Glonass-Kurulum-Kullan%C4%B1m/dp/B0D37YDZNM) 845 ₺
   
3. [Beitian Dual BN-220 GPS GLONASS Anten Modülü TTL Seviye RC Drone Uçak](https://www.elecbee.com/tr-592-GPS-Module/tr-17610-Beitian-Dual-BN-220-GPS-GLONASS-Antenna-Module-TTL-Level-RC-Drone-Airplane 
)$17,59

4. [Tyro119 RC Drone FPV Yarışı için Beitian BN-220T GPS Modülü Çift Modülü](https://www.elecbee.com/tr-617-module-board/tr-17651-Beitian-BN-220T-GPS-Module-Dual-Module-for-Tyro119-RC-Drone-FPV-Racing) $14,32

Ublox ZED-F9P RTK GPS Modülü

-------------------------------------
başka gps modüllerine burdan bakabiliriz : 
https://www.mouser.com.tr/c/embedded-solutions/wireless-rf-modules/gnss-gps-modules/


---
</details>

-----

| **GPS Modülü** | **Kullanılan Protokoller** | **Hazır Kütüphane** | **Yazılım Platformu** | **Driver Gereksinimi** |
| --- | --- | --- | --- | --- |
| **Adafruit Ultimate GPS Breakout** | UART | **Adafruit GPS Kütüphanesi** ([GitHub](https://github.com/adafruit/Adafruit_GPS)) | Arduino, Python | Hayır, kütüphane mevcut. |
|  |  | **TinyGPS++ Kütüphanesi** ([GitHub](https://github.com/mikalhart/TinyGPSPlus)) | Arduino, ESP32, ESP8266 | Hayır, kütüphane mevcut. |
| **Adafruit Ultimate GPS + GLONASS (PA1616D)** | UART | **Adafruit GPS Kütüphanesi** ([GitHub](https://github.com/adafruit/Adafruit_GPS)) | Arduino, Python | Hayır, kütüphane mevcut. |
|  |  | **TinyGPS++ Kütüphanesi** ([GitHub](https://github.com/mikalhart/TinyGPSPlus)) | Arduino, ESP32, ESP8266 | Hayır, kütüphane mevcut. |
| **Ublox GY-GPSV3 NEO-8M** | UART | **u-blox C++/Python API** ([u-blox Docs](https://www.u-blox.com/en/docs/UBX-13003221)) | Arduino, STM32, Raspberry Pi, Python | Hayır, kütüphane mevcut. |
|  |  | **TinyGPS++ Kütüphanesi** ([GitHub](https://github.com/mikalhart/TinyGPSPlus)) | Arduino, ESP32, ESP8266 | Hayır, kütüphane mevcut. |
|  |  | **NeoGPS Kütüphanesi** ([GitHub](https://github.com/SlashDevin/NeoGPS)) | Arduino, AVR, ESP8266 | Hayır, kütüphane mevcut. |
| **Ublox NEO-6M GPS Modülü** | UART | **TinyGPS++ Kütüphanesi** ([GitHub](https://github.com/mikalhart/TinyGPSPlus)) | Arduino, ESP32, ESP8266, STM32 | Hayır, kütüphane mevcut. |
|  |  | **NeoGPS Kütüphanesi** ([GitHub](https://github.com/SlashDevin/NeoGPS)) | Arduino, AVR, ESP8266 | Hayır, kütüphane mevcut. |
|  |  | **u-blox C++/Python API** ([u-blox Docs](https://www.u-blox.com/en/docs/UBX-13003221)) | Arduino, Raspberry Pi, STM32, Python | Hayır, kütüphane mevcut. |
| **Beitian BN-220 GPS Modülü** | UART | **TinyGPS++ Kütüphanesi** ([GitHub](https://github.com/mikalhart/TinyGPSPlus)) | Arduino, ESP32, ESP8266 | Hayır, kütüphane mevcut. |
|  |  | **NeoGPS Kütüphanesi** ([GitHub](https://github.com/SlashDevin/NeoGPS)) | Arduino, AVR, ESP8266 | Hayır, kütüphane mevcut. |
| **Ublox ZED-F9P RTK GPS Modülü** | UART, I2C, SPI | **u-blox F9P RTK Kütüphanesi** ([u-blox Docs](https://www.u-blox.com/en/docs/UBX-21022705)) | Arduino, Raspberry Pi, STM32, Python | Hayır, kütüphane mevcut. |
|  |  | **TinyGPS++ Kütüphanesi** ([GitHub](https://github.com/mikalhart/TinyGPSPlus)) | Arduino, ESP32, ESP8266 | Hayır, kütüphane mevcut. |

### **Kütüphaneler ve Kullanım Alanları:**
| **Kütüphane** | **Platformlar** | **Avantajlar** | **Olası Sorunlar** |
| --- | --- | --- | --- |
| **TinyGPS++** | Arduino, ESP32, ESP8266 | Basit ve hızlı kullanım. Hızlı entegrasyon sağlar. | Bellek kısıtlaması olan projelerde yetersiz olabilir, örneğin düşük bellekli cihazlarda. |
| **NeoGPS** | Arduino, AVR, ESP8266 | Düşük bellek kullanımı, hızlı veri işleme. Gerçek zamanlı verileri işlemek için optimize edilmiştir. | Yapılandırması daha karmaşık olabilir, başlangıç seviyesindeki kullanıcılar için zorlayıcı olabilir. |
| **u-blox API** | Arduino, STM32, Raspberry Pi | Gelişmiş veri işleme, GNSS desteği, yüksek doğruluklu veriler sağlar. | Basit projelerde fazla karmaşık ve donanım gereksinimleri yüksek olabilir. Ayrıca, API'yi kullanmak için biraz daha fazla öğrenme süresi gerekebilir. |
| **u-blox F9P RTK** | Arduino, STM32, Raspberry Pi | RTK (Real-Time Kinematic) desteği, çoklu GNSS Global Navigation Satellite System) sistemlerini destekler, yüksek doğruluk sağlar (santimetre seviyesinde). | Yüksek maliyet, karmaşık yapılandırma, yüksek donanım gereksinimi. |


### 🤔🧐 2025_01_31 / ÖTR Rapor incelemesi ve geçmiş raporları inceleme notlarım: 

- Geçen sene finalist olan takım gps için [bunu](https://www.mouser.com.tr/ProductDetail/u-blox/MAX-8Q-0?qs=DPoM0jnrROXs29tEjk5Xmw%3D%3D) kullanmışlar güç tüketimi bakımından daha az olduğu ve daha hızlı güncelleme hızı için bunu kullandıklarını düşünüyordum Mete başkan " Fazla profesyonel bir ürün , Bunu kullanmamız zor olur. Önce elimizdekiyle işi bitirelim sonra geçmeyi deneriz" dedi.
- 

### 🌚🤝 2025_01_29 / Şartname toplantısı sonrası bazı notlar ve sorular:

- İrtifa 
- Enlem
- Boylam

bilgisi olmak zorunda.

- Jireskoptan gelen verinin birinimi dps mi ?
- Ivmenin birimi g-force mudur ?

yoksa başka bir şey olup onu dönüştürmemiz mi gerekir ?

<a href="(https://github.com/user-attachments/assets/f52d8408-84fa-4977-bce8-330be7c3f8ec)">
    <img src="https://github.com/user-attachments/assets/f52d8408-84fa-4977-bce8-330be7c3f8ec" alt="resim7" width="400" />
  </a>

- USB -TTL olan bağlantı nasıl yapacağız onunla ilgili bir araştırma yapılmalı ? 
- yaptığımız form uygulamasını nasıl export edip onu desktop app yapmamız gerekiyor? 

- HYI denilen sistemin com bağlantı ayarları ile ilgili birkaç madde var onu anlamadık.

### 💥🤙 2025.01.27 / GPS ilk kez çalıştı !.. 

bu [kodu](https://github.com/eren-gokce/racoon/blob/main/gpsTest/gpsModuleVakasi.ino) denedik ve çalıştı. Veri aldık ancak şöyle bir durum var , [NMEA DECODER](https://swairlearn.bluecover.pt/nmea_analyser) da çevirmemiz ve o şekilde Latitude, Longitude, Time çıktılarını anlamamız gerekti.

<a href="https://github.com/user-attachments/assets/7f5c361f-0555-460b-aa46-07284a1dbdd8">
  <img src="https://github.com/user-attachments/assets/7f5c361f-0555-460b-aa46-07284a1dbdd8" alt="resim1" width="450" />
</a>
<a href="https://github.com/user-attachments/assets/16cb8f80-afea-4d90-a05c-b67961d86f9f">
  <img src="https://github.com/user-attachments/assets/16cb8f80-afea-4d90-a05c-b67961d86f9f" alt="resim2" width="450" />
</a>
<a href="https://github.com/user-attachments/assets/33fd836e-89a2-4c3f-a062-ecb3575717f9">
  <img src="https://github.com/user-attachments/assets/33fd836e-89a2-4c3f-a062-ecb3575717f9" alt="resim3" width="450" />
</a>
<a href="https://github.com/user-attachments/assets/0d57ecf8-1266-4181-a1d9-74a077fec470">
  <img src="https://github.com/user-attachments/assets/0d57ecf8-1266-4181-a1d9-74a077fec470" alt="resim4" width="450" />
</a>
<details>
   
<summary> Plan Algoritması: GPS Verisinin Alınması, Decode Edilmesi ve Gösterimi </summary>

#### **1. Başlangıç**
- **Arduino'dan NMEA verisi alınması için gerekli seri port ayarlarını yap.**
  - COM port ve baud rate değerlerini belirle.
  - Seri portu açarak Arduino'dan veri okumayı başlat.

---

#### **2. Veri Alımı**
1. **Arduino'dan gelen veriyi seri port üzerinden oku.**
   - Her satırın bir NMEA cümlesi olduğunu varsay.
   - NMEA cümlelerini satır bazında al (ör. `$GPGLL`, `$GPRMC`).

2. **NMEA cümlelerini bir buffer içine kaydet.**
   - Cümle tamamlandığında (satır sonu `\n` görüldüğünde), decode işlemine geç.

---

#### **3. Decode İşlemi**
1. **Filtreleme:**
   - Gelen veriyi kontrol et.
   - Yalnızca `$GPGLL` veya `$GPRMC` cümlelerini işle.

2. **Veri Ayrıştırma:**
   - `NmeaParser` kullanarak cümleleri otomatik decode et.
     - `$GPGLL`: Latitude, Longitude, UTC Time.
     - `$GPRMC`: Latitude, Longitude, UTC Time ve ek olarak hız/yön bilgisi.
   - Ayrıştırılan değerleri geçerli formatta (ondalık derece) hesapla.

3. **Koordinatları Ondalık Dereceye Dönüştür:**
   - Latitude ve Longitude değerlerini derece/dakika formatından ondalık dereceye çevir.

---

#### **4. Arayüz Güncellemesi**
1. **Enlem, boylam ve zamanı arayüzde göster:**
   - Latitude, Longitude ve Time bilgilerini bir `TextBox` veya `Label` içinde güncelle.

2. **Harita Güncellemesi:**
   - Harita üzerindeki mevcut marker'ı temizle.
   - Yeni Latitude ve Longitude değerleriyle bir marker ekle.
   - Haritayı marker’ın bulunduğu konuma merkezle.

---

#### **5. Verilerin Yedeklenmesi**
1. **Ayrıştırılan tüm verileri bir CSV dosyasına yaz.**
   - Dosya mevcut değilse, başlık satırı ekle: `Latitude,Longitude,Time`.
   - Yeni bir veri alındığında, bu veriyi dosyaya ekle:
     ```
     Latitude, Longitude, Time
     39.968672, 32.743390, 13:45:16
     ```

2. **Veri güvenliği için dosya işlemlerini doğru yönet:**
   - Dosya açıkken hatalardan korunmak için try-catch bloğu kullan.

---

#### **6. Anlık Konum Takibi**
1. **Yeni veri geldiğinde mevcut veriyi geçersiz kıl:**
   - Haritada yalnızca en güncel veriyi göster.
   - Haritayı sürekli yeni gelen veriyle güncelle.

2. **Daha önce gelen tüm verileri CSV dosyasına sakla:**
   - Harita güncellenirken eski veriler kaybolmaz, yalnızca yedeklenir.

---

#### **7. Hata Yönetimi**
1. **Seri Port:**
   - Port açık değilse kullanıcıyı uyar ve programı durdur.
   - Hatalı port seçimi yapılırsa, kullanıcıya uygun mesaj göster.

2. **Decode İşlemi:**
   - Geçersiz bir NMEA cümlesi gelirse, işlemi atla ve bir sonraki satıra geç.

3. **Dosya İşlemleri:**
   - CSV dosyasına yazarken oluşabilecek hataları yönet.

---

#### **8. Programın Sonlandırılması**
- Seri port bağlantısını kapat.
- Haritayı son gösterilen konumda bırak.
- Yedeklenen veriler CSV dosyasında eksiksiz bir şekilde bulunmalı.

---

### **Algoritma Akışı**

```plaintext
Başla
↓
Seri portu aç ve GPS modülünden veri al
↓
Her satırı oku:
    Eğer `$GPGLL` veya `$GPRMC` ile başlıyorsa:
        → NmeaParser kullanarak veriyi ayrıştır
        → Latitude, Longitude ve UTC Time bilgilerini çıkar
        → Koordinatları ondalık dereceye dönüştür
        → Arayüzü güncelle: Enlem, Boylam ve Zaman bilgilerini göster
        → Haritada konumu güncelle: Marker ekle ve haritayı merkeze al
        → Veriyi `gps_data.csv` dosyasına yedekle
    Değilse:
        → Bir sonraki satıra geç
↓
Yeni veri geldikçe işlemi tekrarla
↓
Hata varsa kullanıcıyı bilgilendir
↓
Program sonlandırıldığında:
    → Seri portu kapat
    → Haritayı ve CSV dosyasını kaydet
Bitti
```

---

</details>

<details><summary>Plan: Seri Porttan Gelen NMEA Verisini Otomatik Decode Etme ve İşleme </summary>
   
###### **1. Gerekenler**
- **Kütüphaneler:**
  1. **NmeaParser**:
     - NMEA verilerini otomatik olarak ayrıştırmak için kullanılır.
     - `$GPGLL`, `$GPRMC`, `$GPGGA` gibi standart NMEA cümlelerini destekler.
     - **Yükleme:** 
       ```bash
       Install-Package NmeaParser
       ```
  2. **GMap.NET**:
     - Harita üzerinde konum göstermek için kullanılır.
     - Google Maps, OpenStreetMap gibi sağlayıcılardan veri alır.

- **Donanım:**
  - Arduino + GPS modülü
  - Bilgisayara USB ile bağlı Arduino cihazı

---

###### **2. Uygulama Akışı**

1. **Seri Porttan Veri Alımı:**
   - Arduino, GPS modülünden aldığı NMEA cümlelerini seri port üzerinden bilgisayara gönderir.
   - C# uygulaması, bu verileri seri port dinleyicisiyle alır.

2. **NMEA Verisinin Decode Edilmesi:**
   - Tüm NMEA cümleleri `NmeaParser` ile işlenir.
   - `$GPGLL` veya `$GPRMC` cümlelerinden:
     - **Latitude** (Enlem),
     - **Longitude** (Boylam),
     - **UTC Time** bilgileri ayrıştırılır.

3. **Harita Üzerinde Gösterim:**
   - Ayrıştırılan Latitude ve Longitude bilgileri, **GMap.NET** kullanılarak harita üzerinde marker (işaretleyici) olarak gösterilir.
   - Harita, yeni konuma göre merkezlenir.

4. **Verilerin Yedeklenmesi:**
   - Decode edilen tüm veriler, bir CSV dosyasına şu formatta kaydedilir:
     ```
     Latitude, Longitude, Time
     ```

---

###### **3. Teknik Detaylar**

##### **A. NMEA Verisini Decode Etme**
1. **$GPGLL Cümlesi:**
   - **Format:** `$GPGLL,<latitude>,<N/S>,<longitude>,<E/W>,<UTC time>,<status>,<checksum>`
   - Örnek:
     ```
     $GPGLL,3958.12035,N,03244.60339,E,134516.00,A*61
     ```
   - Ayrıştırılacak Değerler:
     - `Latitude`: 39°58.12035' N → 39.96867267
     - `Longitude`: 32°44.60339' E → 32.74339065
     - `UTC Time`: 13:45:16

2. **$GPRMC Cümlesi:**
   - **Format:** `$GPRMC,<UTC time>,<status>,<latitude>,<N/S>,<longitude>,<E/W>,<speed>,<course>,<date>,...`
   - Örnek:
     ```
     $GPRMC,134517.00,A,3958.12036,N,03244.60344,E,0.601,,270125,,,A*74
     ```
   - Ek olarak:
     - Hız bilgisi `speed` ve yön bilgisi `course` ayrıştırılabilir.

3. **Ondalık Dereceye Dönüştürme:**
   - Derece/Dakika formatını ondalık dereceye dönüştürmek için:
     ```csharp
     private double ConvertToDecimal(string degreeString, string direction)
     {
         double degrees = double.Parse(degreeString.Substring(0, 2));
         double minutes = double.Parse(degreeString.Substring(2));
         double decimalDegrees = degrees + (minutes / 60);

         if (direction == "S" || direction == "W")
             decimalDegrees *= -1;

         return decimalDegrees;
     }
     ```

---

###### **B. Haritada Gösterim**
1. **GMap.NET Harita Ayarları:**
   ```csharp
   private void InitializeMap()
   {
       gMapControl1.MapProvider = GMapProviders.GoogleMap; // Harita sağlayıcı
       GMaps.Instance.Mode = AccessMode.ServerOnly;
       gMapControl1.Position = new PointLatLng(39.92077, 32.85411); // Başlangıç pozisyonu
       gMapControl1.MinZoom = 1;
       gMapControl1.MaxZoom = 20;
       gMapControl1.Zoom = 15;

       markersOverlay = new GMapOverlay("markers");
       gMapControl1.Overlays.Add(markersOverlay);
   }

   private void UpdateMap(double latitude, double longitude)
   {
       markersOverlay.Markers.Clear(); // Eski markerları temizle
       PointLatLng point = new PointLatLng(latitude, longitude);
       GMarkerGoogle marker = new GMarkerGoogle(point, GMarkerGoogleType.red_dot);
       markersOverlay.Markers.Add(marker);
       gMapControl1.Position = point;
   }
   ```

2. **Marker Güncelleme:**
   - Yeni konum geldiğinde marker haritada güncellenir ve harita otomatik olarak merkezlenir.

---

##### **C. Verilerin Yedeklenmesi**
1. **CSV Formatında Kaydetme:**
   ```csharp
   private void SaveToCsv(double latitude, double longitude, string utcTime)
   {
       string filePath = "gps_data.csv";

       if (!File.Exists(filePath))
       {
           File.WriteAllText(filePath, "Latitude,Longitude,Time\n"); // Başlık satırı
       }

       File.AppendAllText(filePath, $"{latitude},{longitude},{utcTime}\n");
   }
   ```

2. **Her Yeni Veri Geldiğinde Kaydetme:**
   - `$GPGLL` veya `$GPRMC` cümlesi decode edildikten sonra, bu fonksiyon çağrılır.

---

##### **4. Özet**
- **Kütüphaneler:**
  1. `NmeaParser`: NMEA cümlelerini decode etmek için.
  2. `GMap.NET`: Harita üzerinde konum göstermek için.

- **Uygulama Akışı:**
  1. Arduino’dan seri port üzerinden NMEA verileri alınır.
  2. `$GPGLL` veya `$GPRMC` cümleleri filtrelenir.
  3. Latitude, Longitude ve Time bilgileri ayrıştırılır.
  4. Decode edilen veriler:
     - Harita üzerinde marker olarak gösterilir.
     - `gps_data.csv` dosyasına kaydedilir.
</details>
<details>

<summary> Örnek kod GPT: </summary>

using System;
using System.IO.Ports;
using System.Windows.Forms;
using GMap.NET;
using GMap.NET.MapProviders;
using GMap.NET.WindowsForms;
using GMap.NET.WindowsForms.Markers;
using System.IO;
using NmeaParser;
using NmeaParser.Messages;
public partial class MainForm : Form
{
    private SerialPort serialPort;
    private GMapOverlay markersOverlay;

    public MainForm()
    {
        InitializeComponent();
        InitializeMap();
    }

    // 1. Harita Başlangıç Ayarları
    private void InitializeMap()
    {
        gMapControl1.MapProvider = GMapProviders.GoogleMap; // Google Maps sağlayıcı
        GMaps.Instance.Mode = AccessMode.ServerOnly;
        gMapControl1.Position = new PointLatLng(39.92077, 32.85411); // Ankara başlangıç pozisyonu
        gMapControl1.MinZoom = 1;
        gMapControl1.MaxZoom = 20;
        gMapControl1.Zoom = 15;

        markersOverlay = new GMapOverlay("markers");
        gMapControl1.Overlays.Add(markersOverlay);
    }

    // 2. Seri Port Bağlantısını Aç
    private void btnConnect_Click(object sender, EventArgs e)
    {
        serialPort = new SerialPort
        {
            PortName = cmbPorts.Text, // Kullanıcıdan seçilen COM port
            BaudRate = 9600,
            DataBits = 8,
            Parity = Parity.None,
            StopBits = StopBits.One
        };

        serialPort.DataReceived += SerialPort_DataReceived; // Veri geldiğinde tetiklenecek
        serialPort.Open();
        MessageBox.Show("Bağlantı Başarılı!");
    }

    // 3. Seri Port Bağlantısını Kapat
    private void btnDisconnect_Click(object sender, EventArgs e)
    {
        if (serialPort != null && serialPort.IsOpen)
        {
            serialPort.Close();
            MessageBox.Show("Bağlantı Kapatıldı!");
        }
    }

    // 4. Seri Port Üzerinden Veri Al
    private void SerialPort_DataReceived(object sender, SerialDataReceivedEventArgs e)
    {
        string nmeaData = serialPort.ReadLine();
        try
        {
            var message = NmeaMessage.Parse(nmeaData); // NmeaParser ile ayrıştırma

            if (message is Gll gll) // Eğer $GPGLL mesajı geldiyse
            {
                ProcessGllMessage(gll);
            }
            else if (message is Rmc rmc) // Eğer $GPRMC mesajı geldiyse
            {
                ProcessRmcMessage(rmc);
            }
        }
        catch (Exception ex)
        {
            Console.WriteLine($"Decode hatası: {ex.Message}");
        }
    }

    // 5. GPGLL Mesajını İşle
    private void ProcessGllMessage(Gll gll)
    {
        double latitude = gll.Latitude;
        double longitude = gll.Longitude;
        string time = gll.Time.ToString();

        // Arayüz Güncelle
        UpdateUI(latitude, longitude, time);

        // Haritada Göster
        UpdateMap(latitude, longitude);

        // CSV'ye Kaydet
        SaveToCsv(latitude, longitude, time);
    }

    // 6. GPRMC Mesajını İşle
    private void ProcessRmcMessage(Rmc rmc)
    {
        double latitude = rmc.Latitude;
        double longitude = rmc.Longitude;
        string time = rmc.FixTime.ToString();

        // Arayüz Güncelle
        UpdateUI(latitude, longitude, time);

        // Haritada Göster
        UpdateMap(latitude, longitude);

        // CSV'ye Kaydet
        SaveToCsv(latitude, longitude, time);
    }

    // 7. Arayüzü Güncelle
    private void UpdateUI(double latitude, double longitude, string time)
    {
        this.Invoke((MethodInvoker)delegate
        {
            txtLatitude.Text = latitude.ToString("F6");
            txtLongitude.Text = longitude.ToString("F6");
            txtTime.Text = time;
        });
    }

    // 8. Haritayı Güncelle
    private void UpdateMap(double latitude, double longitude)
    {
        markersOverlay.Markers.Clear(); // Önceki markerları temizle
        PointLatLng point = new PointLatLng(latitude, longitude);
        GMarkerGoogle marker = new GMarkerGoogle(point, GMarkerGoogleType.red_dot);
        markersOverlay.Markers.Add(marker);
        gMapControl1.Position = point;
    }

    // 9. CSV Dosyasına Kaydet
    private void SaveToCsv(double latitude, double longitude, string utcTime)
    {
        string filePath = "gps_data.csv";

        if (!File.Exists(filePath))
        {
            File.WriteAllText(filePath, "Latitude,Longitude,Time\n"); // Başlık satırı
        }

        File.AppendAllText(filePath, $"{latitude},{longitude},{utcTime}\n");
    }
}


</summary>
</details>

----

<details>
   <summary>Yazılım ve Donanım Arasındaki Haberleşme Mantığını anlamak için yaptığım deneylerin döküman repoları :  </summary>
(about one month ago)

1. [LedBlinkingCsharpArduino](https://github.com/besmabakirci1/LedBlinkingCsharpArduino/tree/main)
2. [Real-Time-Data-Visualization-with-BMP280-and-cSharp](https://github.com/besmabakirci1/Real-Time-Data-Visualization-with-BMP280-and-cSharp/tree/master)
--
</details>
<details>
<summary> 2025_01_07 / versiyon hatası ve çalışmayan GPS </summary> 

🕙09:00-15:28

groundStation.cs 26th line there is "private GMapControl gmapcontrol = new ; " line getting unhendeled exception error aldığımız hatayı bir türlü çözemedik versiyon hatasından kaynaklı bir exception error alıyoruz gpt sorduk araştırdık diğer insanların yaptığı repoları inceledim lakin işe yarar bir çıktı alamadık.

Bulduğumuz işe yarayabilir repolar :
------------------------------------
- [1.REPO](https://github.dev/noant/Lazurite/blob/f8575b9de67a86cb28040c8db34052900a726641/Lazurite/LazuriteUI.Windows.Main/Statistics/Views/GeolocationViewImplementation/LocationsView.xaml.cs#L27#L68)
- [2.REPO](https://github.dev/AliFlux/VectorTileRenderer/blob/b76ccdd010bb1f6b6a8c749d8c0f261977013a20/Gmap.Demo.WinForms/Form1.cs#L16#L40)
- [3.REPO](https://github.dev/disdain13/PokeRoadie/blob/d40914458baf29796c5d31ba5e2cfe703d536ca3/PokeRoadie/Forms/MainForm.cs#L20#L47)

   Alınan Hata : Could not load type 'GMap.NET.Singleton`1' from assembly 'GMap.NET.Core, Version=2.1.6.0, Culture=neutral, PublicKeyToken=b85b9027b614afef'
      NOT : Hata .Net uzantılı bir proje oluşturduğumuzda giderildi. Ceren Hanıma sevgilerimizle :) 

Gps satın almıştım onu denemeye çalıştık ama açık havada olması gerektiğinden olumlu bir çıktı alamadık, çalışıp çalışmadığını bilmiyoruz.
   
🕞 15:30-18:28  

Yanıp sönmeye başladı ama hiç bir şey yazdırmıyo
Sinyal kontolü için bunu denedim Rx Tx değiştirip 10 11 yaptım.
Rx Tx 10 (eskiden 3 yaptim calismadi port okuyor ama yok ) 
Tx Rx 11 (eskiden 4 yaptim calismadi port okuyor ama yok)
Gnd Gnd
Vcc 5v 

dışarda çok beklememe rağmen çalışmadı, bir yerlerim dondu :/ sağlık olsun ne yapalım efenim.. yarın terasta deneyeceğiz cerenle :) umarım olur.

[Gps Module Vakası Repom](https://github.com/besmabakirci1/gpsModuleVakasi/tree/main)
</details>



# [ Ömer Faruk Çift & Berru Erkul, Once Said ] 

## ⚡️ Roket Uçuş Algoritması Açıklaması
### FLOWCHART ÖTR İÇİN 
![ötr için drawio (1)](https://github.com/user-attachments/assets/f24c6af1-ad4f-4e07-b224-e7d90b5a494d)




### FLOWCHART Bizim için
![roketcimms drawio](https://github.com/user-attachments/assets/66848fb9-5c2b-4711-82d9-dc0a1c9d0c85)




### Kütüphaneler
Kodda kullanılan temel kütüphaneler şunlardır:

- `Adafruit_MPU6050.h`: **MPU6050** ivmeölçer ve jiroskop sensörü için Adafruit kütüphanesi.
- `Adafruit_BMP280.h`: **BMP280** barometrik basınç sensörü için Adafruit kütüphanesi.
- `Adafruit_Sensor.h`: Adafruit'in sensör arabirimi.
- `Wire.h`: **I2C** haberleşme için kullanılan kütüphane.
- `SPI.h`: **SPI** haberleşme kütüphanesi.

## Değişkenler ve Kalman Filtresi

<details> <summary> Değişkenler </summary>
  
```cpp
float Q = 0.001; // Süreç gürültü kovaryansı
float R = 0.03;  // Ölçüm gürültü kovaryansı
float P = 1;     // Hata kovaryansı
float K;         // Kalman kazancı
float filteredX = 0, filteredY = 0, filteredZ = 0;
float axOffset = 0, ayOffset = 0, azOffset = 0;
float gxOffset = 0, gyOffset = 0, gzOffset = 0;
float baseAltitude = 0;
float h = bmp.readAltitude(1030.9);
float a = sqrt(pow(axOffset, 2) + pow(ayOffset, 2) + pow(azOffset, 2));
```

- **Kalman Filtresi** kullanılarak sensör verileri gürültüden arındırılır.
- **Kovaryans**, iki değişkenin birbirleriyle nasıl değiştiğini ölçen bir istatistiksel değerdir; pozitifse aynı yönde, negatifse ters yönde değiştiklerini gösterir.
</details>


<details> <summary> Kalman Filtre Fonksiyonu </summary>

  ```cpp
float applyKalmanFilter(float measurement, float &estimate) {
    P += Q;
    K = P / (P + R);
    estimate = estimate + K * (measurement - estimate);
    P *= (1 - K);
    return estimate;
}
```

- `applyKalmanFilter` fonksiyonu, ölçülen veriyi süzerek daha doğru bir tahmin üretir.

</details>

## Sensör Kalibrasyonu
<details> <summary> Sensör Kalibrasyon Kodu </summary>

```cpp
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
    baseAltitude = bmp.readAltitude(1030.9);
    Serial.println("Kalibrasyon tamamlandı.");
}
```
- Sensör verilerinin **ortalaması** alınarak **gürültü azaltılır**.
- **MPU6050** ve **BMP280** için referans değerler belirlenir.
  
</details>

## MPU ve BMP Kontrol Fonksiyon

<details><summary> MPU ve BMP Kontrol Fonksiyon Kodu </summary>
  
Bu fonksiyonlar **MPU6050** ve **BMP280** sensörlerinin çalışıp çalışmadığını kontrol eder.

```cpp
void MPU() {
    for (int i=0; i<3; i++) {
        if (!mpu.begin()) {
            Serial.println("Failed to find MPU6050 chip");
            MPU(); // Çalışmazsa tekrar başlat
        }
    }
    Serial.println("MPU çalışmaya hazır");
}
```

```cpp
void BMP() {
    unsigned status = bmp.begin(0x76);
    for (int i=0; i<3; i++) {
        if (!status) {
            Serial.println(F("BMP280 sensörü bulunamadı!"));
            while (1) delay(10);
        }
    }
}
```
</details>


## Uçuş Algoritması Fonksiyonları

<details><summary> roketHareketEtmiyor, LiffOff , Apogee , Paraşüt Açılma Kontrolü , Roket Yere İndiğinde Kontrol </summary>

### Roket Hareket Etmezse
```cpp
void roketHareketEtmiyor() {
    if(h==){
    axOffset = sensor.acceleration.x;//offsetleri sabitliyoruz değer alınmıyo
    ayOffset = sensor.acceleration.y;
    azOffset = sensor.acceleration.z;
    gxOffset = sensor.gyro.x;
    gyOffset = sensor.gyro.y;
    gzOffset = sensor.gyro.z;
}
    else{ // uçmaya başlarsa kalibrasyondaki ofsetleri bir kere alıp devam ediyoruz sensor kalibrasyonundaki kod
        mpu.getEvent(&a, &g, &temp);
        axOffset += a.acceleration.x;
        ayOffset += a.acceleration.y;
        azOffset += a.acceleration.z;
        gxOffset += g.gyro.x;
        gyOffset += g.gyro.y;
        gzOffset += g.gyro.z;
        delay(500);

    axOffset /= calibrationLoops;
    ayOffset /= calibrationLoops;
    azOffset /= calibrationLoops;
    gxOffset /= calibrationLoops;
    gyOffset /= calibrationLoops;
    gzOffset /= calibrationLoops;

    referansYukseklik = bmp.readAltitude(1030.9); // Referans yükseklik alınması
    }
```
 Eğer roket kalibrasyonu tamamladıktan sonra uçuşa geçmezse uygulanacak tarife   
 
### Liff Off 

```cpp
void LiffOff(){// Başlangıç noktası

for (int i=0; i<3; i++){
  if (h>referansYukseklik && a> kalibrasyondan alınan ivme){// ivmeyi bilemedik 
    delay(100);
  }
  else {
    roketHareketEtmiyor();
    delay(10000);
    LiffOff();
  }
}
  Serial.println("ucus saglandi");
}
}
```
 Roketin belirli bir yükseklik ve ivme eşiğini geçtiğinde uçuş moduna geçtiğini belirler.


### Apogee (Tepe Noktası)
  
```cpp
void apogee() {
    for (int i=0; i<3; i++) {
        if (pitch < 0 && dikey hız <= 0 && h1 < h2) {
            delay(500);
        } else {
            delay(500);
            apogee();
        }
    }
    Serial.println("Apogee ulaşıldı");
}
```
 Roketin **apogee'ye ulaşıp ulaşmadığını** kontrol eder.
  
### Paraşüt Açılma Kontrolü

  ```cpp
void parasut2() {
    for (int i=0; i<3; i++) {
        if (400 < h && h < 600) {
            delay(500);
        } else {
            delay(500);
            parasut2();
        }
    }
    Serial.println("İkinci paraşüt açılabilir");
}
```

 **400m ile 600m** arasında ikinci paraşütün açılmasına izin verir.
  

### Roket Yere İndiğinde Kontrol  
  
```cpp
void alcalmaKontrol() {
    for (int i=0; i<3; i++) {
        if (h1 == h2 && dikey hız == 0 && a == 0) {
            delay(500);
        } else {
            delay(500);
            alcalmaKontrol();
        }
    }
    Serial.println("Roket yerde, GPS verisi alın");
}
```


</details>

## Ana `setup` ve `loop`
  
### `setup` Fonksiyonu
- Sensörleri başlatır.
- Kalibrasyon işlemlerini gerçekleştirir.
- `MPU`, `BMP`, `GPS` ve `lora` modüllerini kontrol eder.

### `loop` Fonksiyonu
- **MPU6050** ve **BMP280**'den verileri okur ve **Kalman filtresi** uygular.
- Uçuşun her aşamasını denetleyerek **uçuş kontrolü**, **apogee**, **paraşüt açılma** ve **iniş kontrolü** fonksiyonlarını çalıştırır.
- Roketin yere indiğinde GPS verisini kaydeder.

## 🪡 EEPROM, **GPS** ve **LoRa** modüllerinin entegrasyonu daha sonra eklenecek.
----

# [ Eren Gökçe, Once Said ] 

## [Yaptıklarımı not aldığım link](https://docs.google.com/document/d/1Ugchummm-yzFRAeQpJfb2bQv5UT_bcpwTwR8XtCUR0M/edit?usp=sharing)

</details>




- [Teknofest şartnamesi](https://cdn.teknofest.org/media/upload/userFormUpload/TEKNOFEST-2025_Roket_Yar%C4%B1%C5%9Fmas%C4%B1_%C5%9Eartnamesi_PfkB5.pdf)

- [Hakem yer istasyonu şartnamesi](https://github.com/user-attachments/files/18579452/EK-7_Hakem_Yer_Istasyonu_y52A5.1.docx)

- [Yeni Arayüz](https://github.com/besmabakirci1/RocketUI2025)
