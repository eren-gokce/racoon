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
