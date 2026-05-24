#ifndef TASK_KIEM_SOAT_H
#define TASK_KIEM_SOAT_H
#include "config.h"
#include <MFRC522.h>
#include <ESP32Servo.h>
#include <Preferences.h> 

extern MFRC522 rfidVao;
extern MFRC522 rfidRa;
extern Servo barrierVao; extern Servo barrierRa;  

String danh_sach_xe[10];            
int so_luong_the = 0;
int che_do_quan_ly = 0; 
Preferences prefs; 

SemaphoreHandle_t spi_mutex = NULL;   
SemaphoreHandle_t servo_mutex = NULL; 

void cap_nhat_flash() {
  prefs.putInt("count", so_luong_the);
  for (int i = 0; i < 10; i++) {
    String key = "c" + String(i);
    if (i < so_luong_the) prefs.putString(key.c_str(), danh_sach_xe[i]);
    else prefs.remove(key.c_str()); 
  }
}

void load_du_lieu_tu_flash() {
  prefs.begin("parking", false); 
  so_luong_the = prefs.getInt("count", 0);
  if (so_luong_the == 0) {
    danh_sach_xe[0] = "63:07:E2:0C"; danh_sach_xe[1] = "6E:8A:42:03";
    danh_sach_xe[2] = "4B:F1:30:02"; danh_sach_xe[3] = "03:4D:E1:0C";
    danh_sach_xe[4] = "A9:09:44:03";
    so_luong_the = 5; cap_nhat_flash();
  } else {
    for (int i = 0; i < so_luong_the; i++) {
      String key = "c" + String(i);
      danh_sach_xe[i] = prefs.getString(key.c_str(), "");
    }
  }
}

// =========================================================
// TASK 1: QUẢN LÝ CỔNG VÀO
// =========================================================
void TaskCongVao(void *pvParameters) {
  load_du_lieu_tu_flash(); 
  spi_mutex = xSemaphoreCreateMutex(); 
  servo_mutex = xSemaphoreCreateMutex(); 

  // Setup chân cơ bản
  pinMode(SS_PIN_VAO, OUTPUT); digitalWrite(SS_PIN_VAO, HIGH);
  pinMode(PIN_IR_VAO, INPUT_PULLUP); 
  barrierVao.attach(PIN_SERVO_VAO); barrierVao.write(0);

  // --- KHỞI ĐỘNG MẮT VÀO ĐÚNG 1 LẦN DUY NHẤT Ở ĐÂY ---
  vTaskDelay(100 / portTICK_PERIOD_MS);
  xSemaphoreTake(spi_mutex, portMAX_DELAY);
  rfidVao.PCD_Init(); 
  xSemaphoreGive(spi_mutex);

  for (;;) {
    String uid = ""; bool co_the = false;
    
    // --- TRONG VÒNG LẶP CHỈ KIỂM TRA THẺ, KHÔNG INIT NỮA ---
    xSemaphoreTake(spi_mutex, portMAX_DELAY);
    if (rfidVao.PICC_IsNewCardPresent() && rfidVao.PICC_ReadCardSerial()) {
      for (byte i = 0; i < rfidVao.uid.size; i++) {
        uid += String(rfidVao.uid.uidByte[i] < 0x10 ? "0" : "");
        uid += String(rfidVao.uid.uidByte[i], HEX);
        if (i < rfidVao.uid.size - 1) uid += ":";
      }
      co_the = true; 
      rfidVao.PICC_HaltA(); rfidVao.PCD_StopCrypto1();
    }
    xSemaphoreGive(spi_mutex); 

    if (co_the) {
      uid.toUpperCase(); uid.trim();
      
      // Tính năng Thẻ Master
      if (uid == "81:C4:6F:16") { 
        che_do_quan_ly = (che_do_quan_ly + 1) % 3;
        if (che_do_quan_ly == 1) trang_thai_he_thong = "CHEDO: THEM THE     ";
        else if (che_do_quan_ly == 2) trang_thai_he_thong = "CHEDO: XOA THE      ";
        else trang_thai_he_thong = "CHEDO: BINH THUONG  ";
        vTaskDelay(1500 / portTICK_PERIOD_MS); trang_thai_he_thong = "AN TOAN             ";
        continue;
      }

      if (che_do_quan_ly == 1) { 
        bool trung = false;
        for(int i=0; i<so_luong_the; i++) if(uid == danh_sach_xe[i]) trung = true;
        if (!trung && so_luong_the < 10) {
          danh_sach_xe[so_luong_the++] = uid; cap_nhat_flash();
          trang_thai_he_thong = "DA THEM THE!        ";
        } else { trang_thai_he_thong = "THE DA CO!          "; }
        vTaskDelay(1500 / portTICK_PERIOD_MS); trang_thai_he_thong = "AN TOAN             ";
      }
      else if (che_do_quan_ly == 2) { 
        int vi_tri = -1;
        for(int i=0; i<so_luong_the; i++) if(uid == danh_sach_xe[i]) vi_tri = i;
        if (vi_tri != -1) {
          for(int i = vi_tri; i < so_luong_the - 1; i++) danh_sach_xe[i] = danh_sach_xe[i+1];
          so_luong_the--; cap_nhat_flash();
          trang_thai_he_thong = "DA XOA THE!         ";
        } else { trang_thai_he_thong = "THE KHONG CO!       "; }
        vTaskDelay(1500 / portTICK_PERIOD_MS); trang_thai_he_thong = "AN TOAN             ";
      }
      else { 
        int idx = -1;
        for (int i = 0; i < so_luong_the; i++) if (uid == danh_sach_xe[i]) { idx = i; break; }

        if (idx != -1) {
          if (!xe_dang_trong_bai[idx]) {
            if (so_cho_trong > 0) { 
              trang_thai_he_thong = "MOI VAO             "; 
              so_cho_trong--; xe_dang_trong_bai[idx] = true;
              
              xSemaphoreTake(servo_mutex, portMAX_DELAY); 
              barrierVao.write(90); 
              vTaskDelay(300 / portTICK_PERIOD_MS);       
              xSemaphoreGive(servo_mutex);                
              
              while(digitalRead(PIN_IR_VAO) == HIGH) { vTaskDelay(20 / portTICK_PERIOD_MS); } 
              while(digitalRead(PIN_IR_VAO) == LOW) { vTaskDelay(20 / portTICK_PERIOD_MS); }  
              vTaskDelay(500 / portTICK_PERIOD_MS);

              xSemaphoreTake(servo_mutex, portMAX_DELAY); 
              barrierVao.write(0); 
              vTaskDelay(300 / portTICK_PERIOD_MS);       
              xSemaphoreGive(servo_mutex);

              trang_thai_he_thong = "AN TOAN             ";
            } else {
              trang_thai_he_thong = "HET CHO!            "; 
              vTaskDelay(2000 / portTICK_PERIOD_MS); trang_thai_he_thong = "AN TOAN             ";
            }
          } else { trang_thai_he_thong = "DA TRONG BAI        "; vTaskDelay(1500 / portTICK_PERIOD_MS); trang_thai_he_thong = "AN TOAN             "; }
        } else { trang_thai_he_thong = "THE LA!             "; vTaskDelay(1000 / portTICK_PERIOD_MS); trang_thai_he_thong = "AN TOAN             "; }
      }
    }
    vTaskDelay(30 / portTICK_PERIOD_MS);
  }
}

// =========================================================
// TASK 2: QUẢN LÝ CỔNG RA
// =========================================================
void TaskCongRa(void *pvParameters) {
  while(spi_mutex == NULL || servo_mutex == NULL) { vTaskDelay(10 / portTICK_PERIOD_MS); }
  
  pinMode(SS_PIN_RA, OUTPUT); digitalWrite(SS_PIN_RA, HIGH);
  pinMode(PIN_IR_RA, INPUT_PULLUP); 
  barrierRa.attach(PIN_SERVO_RA); barrierRa.write(0);

  // --- KHỞI ĐỘNG MẮT RA ĐÚNG 1 LẦN DUY NHẤT Ở ĐÂY ---
  vTaskDelay(100 / portTICK_PERIOD_MS);
  xSemaphoreTake(spi_mutex, portMAX_DELAY);
  rfidRa.PCD_Init(); 
  xSemaphoreGive(spi_mutex);

  for (;;) {
    String uid = ""; bool co_the = false;
    
    // --- TRONG VÒNG LẶP CHỈ KIỂM TRA THẺ ---
    xSemaphoreTake(spi_mutex, portMAX_DELAY);
    if (rfidRa.PICC_IsNewCardPresent() && rfidRa.PICC_ReadCardSerial()) {
      for (byte i = 0; i < rfidRa.uid.size; i++) {
        uid += String(rfidRa.uid.uidByte[i] < 0x10 ? "0" : "");
        uid += String(rfidRa.uid.uidByte[i], HEX);
        if (i < rfidRa.uid.size - 1) uid += ":";
      }
      co_the = true; 
      rfidRa.PICC_HaltA(); rfidRa.PCD_StopCrypto1();
    }
    xSemaphoreGive(spi_mutex); 

    if (co_the) {
      uid.toUpperCase(); uid.trim();
      if (uid == "81:C4:6F:16") { che_do_quan_ly = 0; trang_thai_he_thong = "CHEDO: BINH THUONG  "; vTaskDelay(1500 / portTICK_PERIOD_MS); trang_thai_he_thong = "AN TOAN             "; }
      else if (che_do_quan_ly == 0) {
        int idx = -1;
        for (int i = 0; i < so_luong_the; i++) if (uid == danh_sach_xe[i]) { idx = i; break; }

        if (idx != -1 && xe_dang_trong_bai[idx]) { 
          trang_thai_he_thong = "TAM BIET            "; 
          so_cho_trong++; xe_dang_trong_bai[idx] = false;
          
          xSemaphoreTake(servo_mutex, portMAX_DELAY); 
          barrierRa.write(90); 
          vTaskDelay(300 / portTICK_PERIOD_MS);       
          xSemaphoreGive(servo_mutex);

          while(digitalRead(PIN_IR_RA) == HIGH) { vTaskDelay(20 / portTICK_PERIOD_MS); } 
          while(digitalRead(PIN_IR_RA) == LOW) { vTaskDelay(20 / portTICK_PERIOD_MS); }  
          vTaskDelay(500 / portTICK_PERIOD_MS);

          xSemaphoreTake(servo_mutex, portMAX_DELAY); 
          barrierRa.write(0); 
          vTaskDelay(300 / portTICK_PERIOD_MS);
          xSemaphoreGive(servo_mutex);

          trang_thai_he_thong = "AN TOAN             ";
        } else { trang_thai_he_thong = "THE LA/CHUA VAO     "; vTaskDelay(1000 / portTICK_PERIOD_MS); trang_thai_he_thong = "AN TOAN             "; }
      }
    }
    vTaskDelay(30 / portTICK_PERIOD_MS);
  }
}
#endif