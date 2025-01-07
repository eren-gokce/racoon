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

🕙09:00-15:28

groundStation.cs 26th line there is "private GMapControl gmapcontrol = new ; " line getting unhendeled exception error aldığımız hatayı bir türlü çözemedik versiyon hatasından kaynaklı bir exception error alıyoruz gpt sorduk araştırdık diğer insanların yaptığı repoları inceledim lakin işe yarar bir çıktı alamadık.

bulduğumuz işe yarayabilir repolar : https://github.dev/noant/Lazurite/blob/f8575b9de67a86cb28040c8db34052900a726641/Lazurite/LazuriteUI.Windows.Main/Statistics/Views/GeolocationViewImplementation/LocationsView.xaml.cs#L27#L68
https://github.dev/AliFlux/VectorTileRenderer/blob/b76ccdd010bb1f6b6a8c749d8c0f261977013a20/Gmap.Demo.WinForms/Form1.cs#L16#L40
https://github.dev/disdain13/PokeRoadie/blob/d40914458baf29796c5d31ba5e2cfe703d536ca3/PokeRoadie/Forms/MainForm.cs#L20#L47

Alınan Hata : Could not load type 'GMap.NET.Singleton`1' from assembly 'GMap.NET.Core, Version=2.1.6.0, Culture=neutral, PublicKeyToken=b85b9027b614afef'

Gps satın almıştım onu denemeye çalıştık ama açık havada olması gerektiğinden olumlu bir çıktı alamadık, çalışıp çalışmadığını bilmiyoruz.
   
🕞 15:30-18:28  

Yanıp sönmeye başladı ama hiç bir şey yazdırmıyo
Sinyal kontolü için bunu denedim Rx tx değiştirip 10 11 yaptım.
rx tx 10 (eskiden 3 yaptim calismadi port okuyor ama yok ) 
tx rx 11 (eskiden 4 yaptim calismadi port okuyor ama yok)
gnd dnd
vcc 5v 

dışarda çok beklememe rağmen çalışmadı, bir yerlerim dondu :/ sağlık olsun ne yapalım efenim.. yarın terasta deneyeceğiz cerenle :) umarım olur.
   
Gps Module Vakası: https://github.com/besmabakirci1/gpsModuleVakasi/tree/main 
   
-------
(Ceren Sultan Çetin, Once Said)
---
### Arayüz:
https://github.com/CerenSultanCETIN/RocketInterface2.git
2025_01_06 / 🕓 22:00 -03:38 
1. serial com port iletişimi sağlandı. sadece henüz veri göndermiyor
2. Gps modülünden veriler girilecek ancak kurulan kütüphanenşn sürümü uygun indiirlmediği için bende çalışmıyor.

YAPILACAKLAR
1) Gps modülüyle roketin başlangıç ve anlık konumunu arayüzde görüntülenecek. Enlem ve boylam bilgisi de altına yazılacak. Zoom özelliği eklenecek.
2) Basınç, sıcaklık, yükseklik vs. veriler için bir tablo oluşturulacak. Her verinin karşısına anlık olarak ait olduğu veriler girilecek.
3) Roketin fırlatılma grafiği anlık olarak çizilecek. (openrocketdeki simülasyon grafiği gibi)
4) Paraşütlerin açılma bilgisi arayüzde görüntülenecek.
5) Porta veri aktarımı sağlanacak.
-------
(Ömer Faruk Çift, Once Said)
---
###
-------
##(Berru Erkul, Once Said)
---
###
