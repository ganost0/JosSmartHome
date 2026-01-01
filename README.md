# ESP8266 Smart Relay Controller v7.1

**Hệ thống điều khiển rơ-le thông minh với lịch trình, báo thức, và tích hợp Blynk**

[![Version](https://img.shields.io/badge/version-7.1-blue.svg)](RELEASE_NOTES_V7.1.md)
[![Status](https://img.shields.io/badge/status-production%20ready-brightgreen.svg)](RELEASE_FINAL.md)
[![License](https://img.shields.io/badge/license-MIT-blue.svg)](#license)

---

## 🎯 Tính năng chính

### 🔌 Điều khiển Rơ-le
- ✅ **4 rơ-le độc lập** (LIGHT, FAN, PUMP, TV)
- ✅ **Lịch bật/tắt tự động** cho mỗi rơ-le
- ✅ **Điều khiển thời gian thực** qua Serial, Button, hoặc Blynk

### ⏰ Báo thức & Nhắc nhở
- ✅ **2 báo thức** có thể tùy chỉnh
- ✅ **11 loại âm thanh** (error, internet, system, alarm, mode, etc.)
- ✅ **Chế độ Im lặng** tắt tất cả âm thanh

### 📱 Kết nối & Điều khiển
- ✅ **WiFi tự động** (WiFiManager - không cần code lại)
- ✅ **Blynk Cloud** - điều khiển từ ứng dụng di động
- ✅ **Lệnh Serial** - giao tiếp qua Terminal
- ✅ **5 nút bấm cứng** - điều khiển trực tiếp

### ⚡ Hiệu suất
- ✅ **Không-blocking** - không dùng `delay()` gây treo
- ✅ **Phản ứng nhanh** - millisecond precision
- ✅ **Tiết kiệm RAM** - tối ưu cho ESP8266 (80KB available)

### 🔐 Bảo mật & Cấu hình
- ✅ **Token Blynk** lưu trong `config.h` (không commit)
- ✅ **WiFiManager** - lưu credentials vĩnh viễn
- ✅ **Tất cả cấu hình tập trung** trong config.h

## 📦 Cấu trúc Project

```
ESP8266/
├── src/
│   └── main.cpp                  (1023 lines - production code)
├── include/
│   └── config.h                  (Blynk token, GPIO pins, tone definitions)
├── test/
│   └── README                    (Test notes)
├── lib/
│   └── README                    (Library notes)
├── platformio.ini                (Build configuration)
├── README.md                      (📄 File này)
├── QUICK_START.md                (⚡ Hướng dẫn nhanh)
├── QUICK_REFERENCE.md            (📋 Danh sách lệnh)
├── RELEASE_NOTES_V7.1.md         (✅ Chi tiết cải tiến)
├── FEEDBACK_SYNC_V7.1.md         (🔔 Hệ thống feedback)
└── TONE_CONFIG_GUIDE.md          (🎵 Cấu hình âm thanh)
```

## ⚙️ Yêu cầu

### Hardware
- **Board:** ESP8266 NodeMCU v2 (hoặc tương thích)
- **Relay Module:** 4 channel relay (5V)
- **Buzzer:** Passive buzzer (5V)
- **Buttons:** 5 push buttons with pullup
- **LED:** 1x LED for WiFi status
- **Power:** 5V/2A (for relays)

### Software
- **Framework:** Arduino framework for ESP8266
- **Libraries:**
  - Blynk v1.3.2
  - WiFiManager v2.0.17
  - ESP8266WiFi (built-in)
- **Serial:** 9600 baud

### PlatformIO
```bash
pip install platformio
pio platform install espressif8266
```

## � Thiết lập nhanh (3 bước)

### 1️⃣ Cấu hình Blynk Token

Mở `include/config.h` và chỉnh sửa:

```cpp
#define BLYNK_TEMPLATE_ID "TMPLxxxxxx"
#define BLYNK_TEMPLATE_NAME "Smart Home Control"
#define BLYNK_AUTH_TOKEN "Your_Token_Here"
```

### 2️⃣ Tải lên ESP8266

```bash
cd d:\CANnoe\ESP8266
platformio run -t upload -e nodemcuv2
```

### 3️⃣ Kiểm tra kết nối

```bash
platformio device monitor -b 9600
```

Kỳ vọng thấy:
```
✓ WiFi Connected: [Your_SSID]
  IP: 192.168.x.x
✓ Blynk: Connecting...
✓ Setup Complete
```

## 🎮 Lệnh & Sử dụng

### 📌 Lệnh Rơ-le

```bash
R1 ON              # Bật rơ-le 1 (LIGHT)
R1 OFF             # Tắt rơ-le 1
R1                 # Toggle (bật/tắt) rơ-le 1

R2 ON              # Bật rơ-le 2 (FAN)
R3 OFF             # Tắt rơ-le 3 (PUMP)
R4                 # Toggle rơ-le 4 (TV)
```

### 📅 Lệnh Lịch trình

```bash
SC1 ON 21:00 OFF 06:00    # Rơ-le 1: Bật 21h, tắt 6h
SC2 ON 19:00 OFF 23:00    # Rơ-le 2: Bật 19h, tắt 23h
SC3 ON 06:00 OFF 18:00    # Rơ-le 3: Bật 6h, tắt 18h
SC4 ON 20:00 OFF 22:00    # Rơ-le 4: Bật 20h, tắt 22h

SC1 OFF                   # Tắt lịch rơ-le 1
```

### 🔔 Lệnh Báo thức

```bash
AL1 06:30         # Báo thức 1: 06h30
AL2 12:00         # Báo thức 2: 12h00
AL1 OFF           # Tắt báo thức 1
AL2 OFF           # Tắt báo thức 2
```

### ⏰ Lệnh Thời gian

```bash
T 15:45           # Đặt giờ: 15h45
T+                # Tăng 1 giờ (cho test)
```

### 🎛️ Lệnh Chế độ

```bash
S+                # Chế độ Im lặng: BẬT (tắt tất cả âm thanh)
S-                # Chế độ Im lặng: TẮT

TIMER+            # Bật lịch trình tự động
TIMER-            # Tắt lịch trình tự động
```

### 🔧 Lệnh Hệ thống

```bash
RS                # Reset toàn bộ hệ thống
S  hoặc 1         # Hiển thị trạng thái
H                 # Hiển thị trợ giúp
```

## 🔌 GPIO Pinout

| Chức năng | Pin | GPIO | Ghi chú |
|-----------|-----|------|---------|
| **Relays** | | | |
| Relay 1 | D1 | GPIO5 | LIGHT |
| Relay 2 | D2 | GPIO4 | FAN |
| Relay 3 | D4 | GPIO14 | PUMP |
| Relay 4 | D6 | GPIO12 | TV |
| **Buttons** | | | |
| Button 1 | D3 | GPIO0 | Relay 1 |
| Button 2 | SD3 | GPIO10 | Relay 2 |
| Button 3 | D7 | GPIO13 | Relay 3 |
| Button 4 | RX | GPIO3 | Relay 4 |
| Button 5 | D8 | GPIO15 | Reset (hold 5s) |
| **Other** | | | |
| Buzzer | D5 | GPIO2 | Âm thanh feedback |
| WiFi LED | D0 | GPIO16 | Trạng thái WiFi |

## 📱 Blynk App Configuration

### Virtual Pins

| Pin | Widget | Chức năng | Loại |
|-----|--------|----------|------|
| **V0-V3** | Button (Toggle) | Điều khiển Relay 1-4 | Read/Write |
| **V10** | Styled Button | Gửi lệnh | Write |
| **V20** | Terminal | Nhận phản hồi | Read |

### Cấu hình Button V0 (Relay 1)

1. Kéo **Button** widget vào canvas
2. **Label:** "LIGHT"
3. **Virtual Pin:** V0
4. **Switch Type:** Toggle
5. **ON value:** 1, **OFF value:** 0

### Cấu hình Input V10 (Command)

1. Kéo **Styled Button** hoặc **Input Box** widget
2. **Virtual Pin:** V10
3. Gửi lệnh: `R1 ON`, `SC1 ON 21:00 OFF 06:00`, etc.

### Cấu hình Terminal V20 (Feedback)

1. Kéo **Terminal** widget vào canvas
2. **Virtual Pin:** V20 (read-only)
3. Sẽ tự động nhận tất cả feedback từ lệnh

## 🎵 Âm thanh (11 loại)

Tất cả tone durations có thể tùy chỉnh trong `config.h`:

| Âm thanh | Sử dụng | Ý nghĩa |
|----------|--------|---------|
| **Click** | Lệnh thành công | ✓ Phản hồi tích cực |
| **Error** | Lệnh sai | ❌ Phản hồi lỗi |
| **Internet Connect** | WiFi/Blynk connected | 🌐 Kết nối OK |
| **Internet Disconnect** | WiFi/Blynk lost | 🌐 Mất kết nối |
| **Mode Enter** | Vào chế độ đặc biệt | 🎛️ Mode activation |
| **Mode Exit** | Thoát chế độ đặc biệt | 🎛️ Mode deactivation |
| **System Startup** | Boot/Reset | 🔄 Khởi động |
| **Alarm Chime** | Mỗi giờ (6h-21h) | ⏰ Hourly reminder |
| **Alarm Sleep** | Báo thức sắp tới | 🛌 Nhắc ngủ |
| **Alarm** | Báo thức phát nổ | 🔔 Alerted! |
| **Double Click** | Xác nhận | ✅ Confirmation |

> 💡 Chi tiết: Xem [TONE_CONFIG_GUIDE.md](TONE_CONFIG_GUIDE.md)

---

## 🔄 Feedback & Đồng bộ hóa

Tất cả lệnh trả về **feedback giống nhau** trên cả Serial và Blynk V20:

```
Serial Command: R1 ON
↓
Serial Output:  [Relay 1] LIGHT → ON
Blynk V20:      [Relay 1] LIGHT → ON
```

**Các điểm feedback được đồng bộ:**
- ✅ Điều khiển rơ-le (R1-R4)
- ✅ Lịch trình (SC1-SC4)
- ✅ Báo thức (AL1-AL2)
- ✅ Thời gian (T, T+)
- ✅ Chế độ (S+, S-, TIMER+/-)
- ✅ Reset (RS)
- ✅ Lỗi/unknown commands

> 📖 Chi tiết: Xem [FEEDBACK_SYNC_V7.1.md](FEEDBACK_SYNC_V7.1.md)

## 🐛 Khắc phục sự cố

### ❌ WiFi không kết nối

**Vấn đề:** Không thấy `✓ WiFi Connected` trên Serial

**Giải pháp:**
1. Chắc chắn lần đầu tiên, ESP8266 sẽ tạo AP: `ESP_SmartRelay`
2. Điện thoại kết nối vào AP này (password: `12345678`)
3. Mở trình duyệt → `192.168.4.1`
4. Chọn WiFi và nhập password
5. Thiết bị sẽ restart và kết nối

### ❌ Blynk không kết nối

**Vấn đề:** Thấy `WiFi Connected` nhưng Blynk không kết nối

**Giải pháp:**
1. Kiểm tra token trong `include/config.h` đúng không
2. Kiểm tra app Blynk đã bật không
3. Kiểm tra internet connection
4. Xem Serial Monitor để debug

### ❌ Nút bấm không phản ứng

**Vấn đề:** Bấm nút nhưng không có phản ứng

**Giải pháp:**
1. Kiểm tra dây nối và GPIO pins trong `config.h`
2. Kiểm tra nút bấm có công việc không (thử đo điện trở)
3. Kiểm tra `INPUT_PULLUP` có được bật trong code

### ❌ Âm thanh không phát

**Vấn đề:** Không nghe âm thanh feedback

**Giải pháp:**
1. Kiểm tra GPIO buzzer pin đúng không
2. Kiểm tra buzzer có công việc không
3. Kiểm tra chế độ Im lặng: `S-` để tắt silent mode
4. Thử lệnh: `TESTOK` để test âm thanh

### ❌ Lỗi biên dịch

```bash
# Clean và rebuild
pio run -t clean -e nodemcuv2
pio run -e nodemcuv2

# Kiểm tra syntax
pio check
```

## � Cải tiến v7.1

### ✅ WiFi Reconnection Fix
- **Vấn đề:** WiFi không reconnect sau reset
- **Fix:** Loại bỏ `WiFi.disconnect(true)` và sử dụng saved credentials
- **Kết quả:** Instant WiFi reconnect ✓

### ✅ Feedback Synchronization
- **Vấn đề:** Feedback sai trên Blynk
- **Fix:** Unified feedback system via `sendFeedback()` → V20 Terminal
- **Kết quả:** 100% accurate feedback on both Serial & Blynk ✓

### ✅ Tone Configuration
- **Vấn đề:** Hardcoded durations trong code
- **Fix:** All tone constants moved to `config.h`
- **Kết quả:** Easy customization without editing main.cpp ✓

### ✅ Code Cleanup
- Loại bỏ 18 file test không cần thiết
- Giữ lại 5 file documentation thiết yếu
- Code size: tối ưu, 1023 lines, 0 errors

## � Tài liệu thêm

- **[QUICK_START.md](QUICK_START.md)** - Hướng dẫn deploy nhanh 3 bước
- **[QUICK_REFERENCE.md](QUICK_REFERENCE.md)** - Cheat sheet lệnh
- **[RELEASE_NOTES_V7.1.md](RELEASE_NOTES_V7.1.md)** - Chi tiết fixes & improvements
- **[FEEDBACK_SYNC_V7.1.md](FEEDBACK_SYNC_V7.1.md)** - Hệ thống feedback đồng bộ
- **[TONE_CONFIG_GUIDE.md](TONE_CONFIG_GUIDE.md)** - Cấu hình âm thanh chi tiết

---

## 🔒 Bảo mật

### Credentials Management
- ✅ Blynk token trong `config.h` (không commit)
- ✅ WiFi credentials lưu trong EEPROM (WiFiManager)
- ✅ Default AP password: `12345678` (nên thay đổi nếu production)

### Best Practices
```cpp
// ✅ Đúng: Token trong config.h
#define BLYNK_AUTH_TOKEN "secret_token_here"

// ❌ Sai: Hardcode trong code
String token = "secret_token_here"; // Nguy hiểm!
```

---

## 📋 Roadmap (Future Versions)

- [ ] MQTT support for Home Automation
- [ ] Remote firmware update (OTA)
- [ ] Energy consumption monitoring
- [ ] Advanced scheduling (daily/weekly/monthly)
- [ ] Automatic WiFi recovery
- [ ] Mobile app (iOS/Android)

## 👤 Author

**ESP8266 Smart Relay Controller**  
**Version:** 7.1 (Production Ready ✅)  
**Last Updated:** January 2026

---

## 🚀 Quick Links

| Link | Mô tả |
|------|-------|
| [QUICK_START.md](QUICK_START.md) | ⚡ 3 bước setup nhanh |
| [QUICK_REFERENCE.md](QUICK_REFERENCE.md) | 📋 Danh sách lệnh |
| [RELEASE_NOTES_V7.1.md](RELEASE_NOTES_V7.1.md) | ✅ Chi tiết cải tiến |
| [FEEDBACK_SYNC_V7.1.md](FEEDBACK_SYNC_V7.1.md) | 🔔 Feedback system |
| [TONE_CONFIG_GUIDE.md](TONE_CONFIG_GUIDE.md) | 🎵 Âm thanh config |

---

**Happy Coding!** 🎉

Made with ❤️ for IoT Smart Home

