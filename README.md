# Dự Án Bãi Đỗ Xe Thông Minh (Smart Parking System) 🚗

Dự án Bãi đỗ xe thông minh ứng dụng vi điều khiển **ESP32** và hệ điều hành thời gian thực **FreeRTOS** để xử lý đa nhiệm. Hệ thống tích hợp quản lý quẹt thẻ RFID, nhận diện 6 vị trí đỗ xe độc lập, hệ thống chiếu sáng tự động và tính năng an toàn Fail-Safe (tự động mở cổng khi có hỏa hoạn).

## 🌟 Các Tính Năng Nổi Bật

* **Quản Lý Ra/Vào Bằng Thẻ RFID:** Sử dụng 2 module MFRC522 (cổng vào/ra) chạy chung bus SPI. Dữ liệu thẻ (UID) được lưu thẳng vào bộ nhớ Flash (Preferences), không bị mất khi mất điện.
* **Cập Nhật Trạng Thái 6 Vị Trí Đỗ:** Quét liên tục 6 cảm biến hồng ngoại để báo trạng thái từng chuồng đỗ (Trống/Có xe) lên màn hình LCD 2004 theo thời gian thực.
* **Tính Năng Fail-Safe (Báo Cháy An Toàn):** Cảm biến khói MQ2 và nhiệt độ DHT22 liên tục giám sát. Khi nhiệt độ > 60°C hoặc nồng độ khói > 2000, hệ thống tự động ngắt quy trình quẹt thẻ và mở tung 2 Barie để xe thoát hiểm.
* **Chiếu Sáng Thông Minh:** Tích hợp cảm biến ánh sáng, tự động bật toàn bộ đèn bãi xe khi trời tối nhằm tiết kiệm năng lượng.
* **Tối Ưu Đa Nhiệm (FreeRTOS):** Các tác vụ (Task) được chia nhỏ và cấp mức độ ưu tiên (Priority) rõ ràng, giúp hệ thống không bị giật lag hay sụt áp:
  * `Priority 4`: Cảnh báo an toàn (Cao nhất).
  * `Priority 3`: Kiểm soát ra vào (RFID).
  * `Priority 2`: Hiển thị LCD & Quét trạng thái chuồng đỗ.

## 🛠 Phần Cứng Yêu Cầu

* 1x Vi điều khiển ESP32 DevKit V1
* 2x Module RFID MFRC522 (Giao tiếp SPI)
* 2x Động cơ Servo SG90 / MG996R (Barie)
* 8x Cảm biến vật cản hồng ngoại (6 cho chuồng đỗ, 2 cho cổng ra/vào)
* 1x Màn hình LCD 2004 kèm module I2C
* 1x Cảm biến khói MQ2
* 1x Cảm biến nhiệt độ, độ ẩm DHT22
* 1x Cảm biến ánh sáng & Đèn LED báo hiệu
* Nguồn hạ áp LM2596 (Cấp nguồn 5V độc lập cho Servo)

## 📂 Cấu Trúc Thư Mục Mạch (Source Code)

Hệ thống được lập trình theo tư duy Module (chia nhỏ file) để dễ quản lý:

* `main.ino`: Khởi tạo hệ thống, cấp phát tài nguyên và cấu hình các Task FreeRTOS.
* `config.h`: Khai báo thư viện, định nghĩa toàn bộ chân GPIO nối với cảm biến và biến toàn cục.
* `task_kiem_soat.h`: Xử lý logic đọc thẻ RFID, đóng/mở Servo cổng vào và cổng ra.
* `task_an_toan.h`: Quản lý cảm biến DHT22, MQ2, cảm biến ánh sáng và tính năng mở cổng khẩn cấp.
* `task_hien_thi.h`: Giao tiếp I2C hiển thị thông tin lên màn hình LCD 2004.

## 🚀 Hướng Dẫn Cài Đặt & Nạp Code

1. Tải và cài đặt Arduino IDE.
2. Thêm thư viện board ESP32 vào Board Manager.
3. Cài đặt các thư viện cần thiết:
   * `MFRC522` (Giao tiếp RFID)
   * `ESP32Servo` (Điều khiển Servo trên nền ESP32)
   * `LiquidCrystal_I2C` (Hiển thị màn hình)
   * `DHT sensor library` (Đọc cảm biến DHT)
4. Mở file `main.ino`. Đảm bảo các file `.h` nằm chung một thư mục.
5. Chọn Board **DOIT ESP32 DEVKIT V1** và cổng **COM** tương ứng.
6. Bấm **Upload** (Lưu ý nhấn giữ nút `BOOT` trên ESP32 khi màn hình hiện `Connecting...`).

---
*Phát triển bởi: Nghiêm Quốc Anh*
