# Roket Aviyonik 🚀

(Eren Gökçe, Once Said)
---
### Yaptıklarımı not aldığım link:  
https://docs.google.com/document/d/1Ugchummm-yzFRAeQpJfb2bQv5UT_bcpwTwR8XtCUR0M/edit?usp=sharing
-------
Besma Bakırcı, Once Said)
---  
### Yazılım ve Donanım Arasındaki Haberleşme Mantığını anlamak için yaptığım deneylerin döküman repoları :  
(about one month ago)
1. https://github.com/besmabakirci1/LedBlinkingCsharpArduino/tree/main
2. https://github.com/besmabakirci1/Real-Time-Data-Visualization-with-BMP280-and-cSharp/tree/master
--

2025_01_07 /
--------------
🕙09:00-15:28
--------------
groundStation.cs 26th line there is "private GMapControl gmapcontrol = new ; " line getting unhendeled exception error aldığımız hatayı bir türlü çözemedik versiyon hatasından kaynaklı bir exception error alıyoruz gpt sorduk araştırdık diğer insanların yaptığı repoları inceledim lakin işe yarar bir çıktı alamadık.

bulduğumuz işe yarayabilir repolar : https://github.dev/noant/Lazurite/blob/f8575b9de67a86cb28040c8db34052900a726641/Lazurite/LazuriteUI.Windows.Main/Statistics/Views/GeolocationViewImplementation/LocationsView.xaml.cs#L27#L68
https://github.dev/AliFlux/VectorTileRenderer/blob/b76ccdd010bb1f6b6a8c749d8c0f261977013a20/Gmap.Demo.WinForms/Form1.cs#L16#L40
https://github.dev/disdain13/PokeRoadie/blob/d40914458baf29796c5d31ba5e2cfe703d536ca3/PokeRoadie/Forms/MainForm.cs#L20#L47

Alınan Hata : Could not load type 'GMap.NET.Singleton`1' from assembly 'GMap.NET.Core, Version=2.1.6.0, Culture=neutral, PublicKeyToken=b85b9027b614afef'
NOT : Hata .Net uzantılı bir proje oluşturduğumuzda giderildi. Ceren Hanıma sevgilerimizle :) 

Gps satın almıştım onu denemeye çalıştık ama açık havada olması gerektiğinden olumlu bir çıktı alamadık, çalışıp çalışmadığını bilmiyoruz.
   
🕞 15:30-18:28  
---------------
Yanıp sönmeye başladı ama hiç bir şey yazdırmıyo
Sinyal kontolü için bunu denedim Rx tx değiştirip 10 11 yaptım.
Rx Tx 10 (eskiden 3 yaptim calismadi port okuyor ama yok ) 
Tx Rx 11 (eskiden 4 yaptim calismadi port okuyor ama yok)
Gnd Gnd
Vcc 5v 

dışarda çok beklememe rağmen çalışmadı, bir yerlerim dondu :/ sağlık olsun ne yapalım efenim.. yarın terasta deneyeceğiz cerenle :) umarım olur.
   
[Gps Module Vakası Repo](https://github.com/besmabakirci1/gpsModuleVakasi/tree/main)

2025_01_25
-----------
GPS için araştırma sürecine girdim ancak araştırırken birkez daha aldığım GPS modülunu denemeye karar verdim, başka bir kodla EKTEDİR, Cerendeydi GPS getirdi bende test için eve getirdim. Pazartesi günü Yusuf ve Şenol hocalarla konuşmak için gideceğim Ömer de eşlik edecek inşallah o güne kadar detaylı bir GPS modül araştırma raporlamasını yapmış olur test etmiş olurum yeni kodu umarım lock olur. 

[NEO-6M GPS MODÜL ile konum, hız, irtifa, tarih bilgisi alma | ESP VE ARDUİNO | TİNYGPS++ LİBRARY](https://www.youtube.com/watch?v=yDfn9ZEc5MA)
[Videodaki verilen kod](https://drive.google.com/drive/folders/1Ig9ccjL5o0V9VCOxEw0sJ4Tpg6B3VTsB)
[TinyGPS++ Library](https://github.com/mikalhart/TinyGPSPlus?tab=readme-ov-file)

--
{Elimizdeki GY-NEO6MV2 GPS Modülü}
-----------------------------
Alternatif Modüller :

Ublox GYGPSV1 NEO-8M GPS Modülü
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

Adafruit Ultimate GPS 
--
[ Adafruit Ultimate GPS Breakout - 66 channel w/10 Hz updates: Bu modül, yüksek hassasiyetli GPS verileri sağlar ve 10 Hz'e kadar güncelleme hızı sunar. Ayrıca, dahili veri kaydedici ve antene sahiptir.
](https://www.adafruit.com/product/5440)
----------------------------------------------------- --------
[ Adafruit Ultimate GPS Breakout with GLONASS + GPS - PA1616D: Bu model, hem GPS hem de GLONASS uydu sistemlerini destekler, bu sayede daha hızlı ve hassas konum belirleme imkanı sunar.](https://www.adafruit.com/product/746?utm_source=chatgpt.com)


benim beğendiğim  u-blox NEO-M8N-0 
Roket takımları genelde Adafruit Ultimate GPS Modülü veya elimizdeki GY-NEO6MV2 GPS Modülünü kullanmış 
-------------------------------------
başka gps modüllerine burdan bakabiliriz : 
https://www.mouser.com.tr/c/embedded-solutions/wireless-rf-modules/gnss-gps-modules/
------------------------

Soru : Test istasyonunu hakemin bilgisayarına usb ile nasıl yansıtıcaz
-------
(Ceren Sultan Çetin, Once Said)
---
### Arayüz:https://github.com/CerenSultanCETIN/RocketInterface13.git
![Arayüz](https://github.com/user-attachments/assets/b3c92df3-6dc9-4abe-9a28-f702d46e05a6)




(Ömer Faruk Çift, Once Said)
---
###
-------
(Berru Erkul, Once Said)
---
###
