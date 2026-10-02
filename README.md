# APP-SMARTPHONE-VinFast-EV-Telemetry-CAN-Bus-Tracker
# 🚗 VinFast EV Telemetry & CAN Bus Tracker

Hệ thống theo dõi thời gian và thói quen vận hành xe điện (VinFast EVO200) theo thời gian thực sử dụng **ESP32** để đọc dữ liệu từ mạng **CAN Bus**, sau đó truyền qua **Bluetooth Low Energy (BLE)** tới ứng dụng **Flutter Mobile**.

---

## 📌 Tính năng chính

- **Đọc dữ liệu CAN Bus xe điện (500 kbps):**
  - Trạng thái khóa điện / Chìa khóa (`0x101`).
  - Dung lượng pin SOC (`0x322`).
  - Điện áp tổng Pack Voltage (`0x309`).
  - Trạng thái sạc / vận hành (`0x110` / `0x303`).
- **Theo dõi thời gian vận hành (Trip Hours / Driving Duration):** Tự động tính toán và đếm thời gian chạy thực tế mỗi khi xe bật chìa khóa (`Ignition ON`).
- **Chuyển tiếp dữ liệu không dây:** Đóng gói telemetry dưới dạng JSON tối ưu để gửi qua BLE Notify tới thiết bị di động.
- **Ứng dụng Mobile (Flutter):**
  - Kết nối BLE tự động với ESP32.
  - Hiển thị Dashboard thời gian thực (Trạng thái ON/OFF, Thời gian chuyến đi, % Pin, Điện áp).
  - Giao diện tối màu (Dark Mode) tối ưu hiển thị khi lái xe.

---

## 🏗 Kiến trúc Hệ thống

```
[Mạng CAN Bus Xe EV] 
         │ (Tín hiệu CAN 500kbps)
         ▼
[ESP32 / ESP32-S3 + SN65HVD230 Transceiver]
         │
         ├── Giải mã dữ liệu CAN (TWAI Driver)
         ├── Tính thời gian vận hành
         │
         ▼ (Giao tiếp Bluetooth Low Energy - BLE)
[Mobile App (Flutter UI)]
         │
         ├── Dashboard hiển thị thời gian thực
         └── Quản lý trạng thái bằng Provider
```

---

## 🛠 Phần cứng yêu cầu

1. **Board mạch:** ESP32 / ESP32-S3 DevKit.
2. **Bộ thu phát CAN (Transceiver):** SN65HVD230 hoặc VP230 (kết nối cổng TWAI của ESP32).
3. **Cáp nối:** Jack OBD-II hoặc điểm trích dây CAN_H, CAN_L trên xe VinFast.

### Sơ đồ đấu nối dây (Hardware Wiring)

| Linh kiện SN65HVD230 | Chân kết nối ESP32 |
| :--- | :--- |
| **CTX** | GPIO 5 (TWAI TX) |
| **CRX** | GPIO 4 (TWAI RX) |
| **VCC** | 3.3V |
| **GND** | GND |
| **CAN_H / CAN_L** | Đấu vào đường CAN_H / CAN_L trên xe |

---

## 📂 Cấu trúc Dự án

```text
├── esp32_can_tracker/       # Mã nguồn C++/Arduino cho ESP32
│   └── src/
│       └── main.cpp         # Đọc CAN TWAI & Phát BLE Service
│
└── mobile_app/              # Mã nguồn Flutter App
    ├── lib/
    │   ├── main.dart                 # Khởi chạy ứng dụng & Provider
    │   ├── providers/
    │   │   └── telemetry_provider.dart # Xử lý kết nối BLE & nhận JSON
    │   └── screens/
    │       └── dashboard_screen.dart   # Giao diện Dashboard hiển thị
    └── pubspec.yaml                  # Khai báo phụ thuộc Flutter
```

---

## 🚀 Hướng dẫn Cài đặt & Khởi chạy

### 1. Nạp firmware cho ESP32

1. Mở dự án trong **VS Code (PlatformIO)** hoặc **Arduino IDE**.
2. Đảm bảo đã thêm thư viện hệ thống ESP32 hỗ trợ driver `driver/twai.h` và `BLEDevice.h`.
3. Biên dịch và nạp chương trình vào board ESP32 qua cổng USB-C / Micro-USB.

### 2. Cài đặt Ứng dụng Mobile (Flutter)

1. Tải các gói phụ thuộc:
   ```bash
   cd mobile_app
   flutter pub get
   ```

2. Cấu hình quyền Bluetooth:
   - **Android (`android/app/src/main/AndroidManifest.xml`):** Yêu cầu cấp quyền `BLUETOOTH_SCAN`, `BLUETOOTH_CONNECT`, `ACCESS_FINE_LOCATION`.
   - **iOS (`ios/Runner/Info.plist`):** Thêm quyền `NSBluetoothAlwaysUsageDescription`.

3. Kết nối điện thoại và khởi chạy ứng dụng:
   ```bash
   flutter run
   ```

---

## 📡 Cấu trúc Giao thức BLE Data Payload

ESP32 phát chuỗi định dạng JSON qua BLE Notification (UUID Characteristic: `beb5483e-36e1-4688-b7f5-ea07361b26a8`):

```json
{
  "ign": 1,        // Trạng thái chìa khóa (1: ON, 0: OFF)
  "dur": 1240,     // Thời gian xe đã chạy trong chuyến (giây)
  "soc": 85,       // Dung lượng Pin (%)
  "v": 61.2        // Điện áp Pack (Volt)
}
```

---

## ⚡ Phát triển tiếp theo (Roadmap)

- [ ] Tích hợp thẻ nhớ **MicroSD** trên ESP32 để lưu log offline khi không nối ứng dụng.
- [ ] Thêm tính năng phân tích **Thói quen lái xe** (Số lần phanh gắt, tăng tốc nhanh dựa trên thay đổi dòng/áp).
- [ ] Lưu dữ liệu lịch sử chuyến đi vào cơ sở dữ liệu SQLite/Hive ngay trên ứng dụng Flutter.
