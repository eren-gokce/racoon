/*
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
Roket sistemleri gibi güvenlik-kritik uygulamalarda TWDT, sistemin ana görevlerinin donması, veri akışının kesilmesi veya 
infinite loop gibi yazılım hatalarının oluşması durumunda müdahale ederek sistemin kendi kendini kurtarabilmesini sağlar.
*/

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

  delay(200); // burada zaten delay varmış, sorun değil
}
