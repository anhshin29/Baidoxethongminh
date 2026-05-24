#ifndef TASK_AN_TOAN_H
#define TASK_AN_TOAN_H
#include "config.h"
#include <DHT.h>

extern DHT dht22;

void TaskAnToan(void *pvParameters) {
  dht22.begin();
  pinMode(PIN_KHOI, INPUT);
  
  pinMode(PIN_AS, INPUT);
  pinMode(PIN_DEN, OUTPUT); 
  digitalWrite(PIN_DEN, LOW); 

  for (;;) {
    nhiet_do = dht22.readTemperature();
    do_am = dht22.readHumidity();
    nong_do_khoi = analogRead(PIN_KHOI);

    // --- LOGIC CHIẾU SÁNG (ĐÃ ĐỔI SANG HIGH) ---
    // Nếu che tay (Tối) mà bồ thấy tín hiệu trả về là HIGH
    // thì dùng lệnh này để nó BẬT đèn bãi xe lên
    if (digitalRead(PIN_AS) == HIGH) { 
      digitalWrite(PIN_DEN, HIGH); // Trời tối (Tín hiệu HIGH) -> Bật đèn
    } else {
      digitalWrite(PIN_DEN, LOW);  // Trời sáng (Tín hiệu LOW) -> Tắt đèn
    }

    // --- LOGIC BÁO CHÁY ---
    if (isnan(nhiet_do)) nhiet_do = 0;
    if (isnan(do_am)) do_am = 0;

    if (nhiet_do > 60 || nong_do_khoi > 2000) {
      trang_thai_he_thong = "CHAY!               "; 
    } else {
      if (trang_thai_he_thong == "CHAY!               ") {
        trang_thai_he_thong = "AN TOAN             ";
      }
    }

    vTaskDelay(500 / portTICK_PERIOD_MS); 
  }
}
#endif