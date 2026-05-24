#include <SPI.h>
#include <MFRC522.h>
#include <ESP32Servo.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>

#include "config.h"
#include "task_kiem_soat.h"
#include "task_hien_thi.h"
#include "task_an_toan.h"

MFRC522 rfidVao(SS_PIN_VAO, RST_PIN);
MFRC522 rfidRa(SS_PIN_RA, RST_PIN);
Servo barrierVao; Servo barrierRa;  
LiquidCrystal_I2C lcd(0x27, 20, 4); 
DHT dht22(PIN_DHT, DHT22);

float nhiet_do = 0.0, do_am = 0.0; 
int nong_do_khoi = 0, so_cho_trong = 5; 
String trang_thai_he_thong = "AN TOAN             "; 
bool xe_dang_trong_bai[10] = {false};
bool trang_thai_slot[6] = {false}; 

void TaskDocSlot(void *pvParameters) {
  pinMode(S1_PIN, INPUT_PULLUP); 
  for (;;) {
    trang_thai_slot[0] = (digitalRead(S1_PIN) == LOW); 
    for(int i = 1; i < 6; i++) trang_thai_slot[i] = false;
    vTaskDelay(300 / portTICK_PERIOD_MS); 
  }
}

void setup() {
  SPI.begin();
  xTaskCreate(TaskAnToan, "AnToan", 4096, NULL, 4, NULL); 
  xTaskCreatePinnedToCore(TaskCongVao, "CongVao", 4096, NULL, 3, NULL, 1); 
  xTaskCreatePinnedToCore(TaskCongRa,  "CongRa",  4096, NULL, 3, NULL, 1); 
  xTaskCreate(TaskDocSlot, "DocSlot", 2048, NULL, 2, NULL); 
  xTaskCreate(TaskHienThi, "HienThi", 2048, NULL, 2, NULL);
}

void loop() { vTaskDelay(10 / portTICK_PERIOD_MS); }