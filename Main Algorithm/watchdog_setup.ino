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
