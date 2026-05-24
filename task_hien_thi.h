#ifndef TASK_HIEN_THI_H
#define TASK_HIEN_THI_H
#include "config.h"
#include <LiquidCrystal_I2C.h>

extern LiquidCrystal_I2C lcd;

void TaskHienThi(void *pvParameters) {
  lcd.init(); lcd.backlight();
  lcd.clear();

  for (;;) {
    String tt = trang_thai_he_thong;
    tt.trim(); 

    if (tt == "CHAY!") {
      lcd.setCursor(0, 0); lcd.print("!!!   BAO CHAY   !!!");
      lcd.setCursor(0, 1); lcd.print("N:"); lcd.print((int)nhiet_do); lcd.print("C Kh:"); lcd.print(nong_do_khoi); lcd.print("    ");
      lcd.setCursor(0, 2); lcd.print("THOAT HIEM KHAN CAP "); 
      lcd.setCursor(0, 3); lcd.print("                    "); 
      lcd.noBacklight(); vTaskDelay(200 / portTICK_PERIOD_MS); lcd.backlight(); vTaskDelay(200 / portTICK_PERIOD_MS);
    } 
    else {
      lcd.setCursor(0, 0);
      String strLine0 = "Slot:" + String(so_cho_trong) + "/5 T:" + String((int)nhiet_do) + "C";
      lcd.print(strLine0);
      for(int i = strLine0.length(); i < 20; i++) lcd.print(" ");

      lcd.setCursor(0, 1);
      String strLine1 = "Am:" + String((int)do_am) + "% Kh:" + String(nong_do_khoi);
      lcd.print(strLine1);
      for(int i = strLine1.length(); i < 20; i++) lcd.print(" ");

      lcd.setCursor(0, 2);
      lcd.print(trang_thai_he_thong);

      lcd.setCursor(0, 3);
      lcd.print("P: ");
      for(int i = 0; i < 6; i++) {
        lcd.print(i+1);
        if(trang_thai_slot[i]) lcd.print("x"); else lcd.print(".");
        if(i < 5) lcd.print(" ");
      }
    }
    vTaskDelay(500 / portTICK_PERIOD_MS); 
  }
}
#endif