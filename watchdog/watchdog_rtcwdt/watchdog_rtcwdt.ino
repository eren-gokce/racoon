/*
RTC Watchdog Timer (RTC_WDT), ESP32'nin bootloader aşamasında otomatik olarak başlatılır ve varsayılan olarak uygulama başlamadan 
hemen önce devre dışı bırakılır. Ancak, CONFIG_BOOTLOADER_WDT_DISABLE_IN_USER_CODE yapılandırma seçeneği devre dışı bırakıldığında, 
RTC_WDT uygulama çalışırken de aktif kalır. Bu durumda kullanıcı, sistemin periyodik olarak rtc_wdt_feed() fonksiyonunu çağırarak watchdog’u beslemesi veya 
rtc_wdt_disable() fonksiyonu ile devre dışı bırakması gerekir. Aksi halde sistem, timeout süresi sonunda otomatik olarak resetlenir.
*/

#include "hal/wdt_hal.h"   // wdt_hal_feed(), wdt_hal_disable()
#include "esp_private/rtc_wdt.h"  // rtc_wdt_feed(), rtc_wdt_disable()

void setup() {
  Serial.begin(115200);
  delay(1000);

  // RTC_WDT aktif kalacaksa beslemeye başla
  // (devre dışı bırakmak istersen: rtc_wdt_disable(); yazarsın)
}

void loop() {

  //......

  rtc_wdt_feed();  // Her döngüde RTC_WDT'yi besle

}