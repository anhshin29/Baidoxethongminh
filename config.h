#ifndef CONFIG_H
#define CONFIG_H
#include <Arduino.h>

// --- 1. CHÂN CẢM BIẾN ---
#define PIN_KHOI       34   
#define PIN_DHT        4    
#define PIN_IR_VAO     25   
#define PIN_IR_RA      32   

// --- 2. CHÂN ĐIỀU KHIỂN ---
#define PIN_SERVO_VAO  13   
#define PIN_SERVO_RA   14

// --- 3. CHÂN GIAO TIẾP RFID ---
#define SS_PIN_VAO     5    
#define SS_PIN_RA      17   
#define RST_PIN        2    // 

// --- 4. TÍNH NĂNG MỚI ---
#define S1_PIN         33   // Cảm biến vị trí số 1
#define PIN_AS         27   // <--- Cảm biến ánh sáng chiếm chân của còi cũ
#define PIN_DEN        12   // Đèn LED bãi xe

// --- 5. BIẾN DÙNG CHUNG ---
extern float nhiet_do;
extern float do_am;
extern int nong_do_khoi;
extern String trang_thai_he_thong;
extern int so_cho_trong; 
extern bool xe_dang_trong_bai[10];
extern bool trang_thai_slot[6];

#endif