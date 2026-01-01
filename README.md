# Smart Relay & Alarm System v6.0

**Hệ thống quản lý rơ-le thông minh với lịch bật/tắt, báo thức, và điều khiển qua Blynk**

## 🎯 Tính năng chính

- ✅ **4 Rơ-le** có lịch bật/tắt tự động
- ✅ **2 Báo thức** (mặc định bật)
- ✅ **Hour Chime** (bíp vào mỗi giờ 6h-21h)
- ✅ **Sleep Reminder** (nhắc ngủ 23h-0h)
- ✅ **Điều khiển qua:**
  - Nút bấm cứng (5 nút)
  - Serial command (lệnh nhanh)
  - Blynk app (qua WiFi)
- ✅ **Không dùng `delay()`** - không bị treo
- ✅ **Bảo mật token Blynk** trong `config.h`

## 📦 Cấu trúc project

```
ESP8266/
├── src/
│   ├── main.cpp              ← Code chính (v6.0)
│   └── main_old.cpp          ← Backup version 5.0
├── include/
│   ├── config.h              ← 🔐 Blynk token, GPIO pins
│   ├── buzzer.h              ← Hàm beep (không-blocking)
│   ├── relay.h               ← Điều khiển rơ-le
│   ├── alarm.h               ← Logic báo thức
│   ├── buttons.h             ← Xử lý nút bấm
│   └── commands.h            ← Lệnh Serial
├── platformio.ini            ← Cấu hình build
├── IMPROVEMENTS.md           ← Chi tiết cải tiến
├── GUIDE.md                  ← Hướng dẫn sử dụng
└── README.md                 ← File này
```

## ⚡ Yêu cầu

- **Board:** ESP8266 NodeMCU v2
- **Framework:** Arduino
- **Libraries:**
  - Blynk v1.3.2
  - WiFiManager v2.0.17
  - Built-in ESP8266 WiFi & time libraries
- **Serial:** 9600 baud

## 🔧 Thiết lập nhanh

### 1. Clone/Download project
```bash
cd d:\CANnoe\ESP8266
```

### 2. Cấu hình Blynk token
Mở `include/config.h`:
```cpp
#define BLYNK_TEMPLATE_ID "YOUR_TEMPLATE_ID"
#define BLYNK_TEMPLATE_NAME "Smart Home Control"
#define BLYNK_AUTH_TOKEN "YOUR_AUTH_TOKEN"
```

### 3. Biên dịch
```bash
pio run -e nodemcuv2
```

### 4. Tải lên
```bash
pio run -t upload -e nodemcuv2
```

### 5. Kiểm tra
```bash
pio device monitor -b 9600
```

## 🎮 Lệnh Serial (Mới)

| Lệnh | Ví dụ | Chức năng |
|------|--------|----------|
| **Rơ-le** |
| `R<1-4> ON HH:MM` | `R1 ON 21:00` | Lịch bật rơ-le |
| `R<1-4> OFF HH:MM` | `R2 OFF 06:30` | Lịch tắt rơ-le |
| **Báo thức** |
| `A<1-2> HH:MM` | `A1 06:00` | Đặt báo thức (auto ON) |
| `A<1-2> OFF` | `A1 OFF` | Tắt báo thức |
| **Thời gian** |
| `T HH:MM` | `T 12:00` | Đặt giờ hiện tại |
| `T+` | `T+` | Tăng 1 giờ (test) |
| **Nút bấm** |
| `M` | `M` | Mode button (b1) |
| `R1-R4` | `R1` | Relay buttons (b2-b5) |
| **Hệ thống** |
| `S` | `S` | Status (xem trạng thái) |
| `RS` | `RS` | Reset (khởi động lại) |
| `H` | `H` | Help (xem trợ giúp) |

### Ví dụ cụ thể:
```
Đặt rơ-le 1 bật lúc 21:00:
  → R1 ON 21:00

Đặt báo thức 1 ở 06:00:
  → A1 06:00

Tắt báo thức 2:
  → A2 OFF

Xem trạng thái:
  → S

Xem help:
  → H
```

## 🎛️ GPIO Pins

| Chức năng | Pin | GPIO |
|-----------|-----|------|
| **Rơ-le** |
| Relay 1 | D1 | GPIO5 |
| Relay 2 | D2 | GPIO4 |
| Relay 3 | D4 | GPIO14 |
| Relay 4 | D6 | GPIO12 |
| **Nút** |
| Mode (b1) | SD3 | GPIO10 |
| Relay 1 (b2) | D3 | GPIO0 |
| Relay 2 (b3) | D7 | GPIO13 |
| Relay 3 (b4) | RX | GPIO3 |
| Relay 4 (b5) | D8 | GPIO15 |
| **Other** |
| Buzzer | D5 | GPIO2 |
| WiFi LED | D0 | GPIO16 |

## 📱 Blynk Virtual Pins

| Pin | Chức năng | Type |
|-----|-----------|------|
| V0-V3 | Relay 1-4 | Switch |
| V4 | Timer Control | Switch |
| V5 | Chiming | Switch |
| V6 | Silent Mode | Switch |
| V10 | Terminal | Widget |
| V11 | Terminal + Status | Widget |

## 🔄 Cải tiến v6.0

### ✅ Loại bỏ `delay()` → `millis()`
- **Trước:** Dùng `delay()` làm treo hệ thống
- **Sau:** Dùng `millis()` + `beep_update()` không-blocking
- **Lợi ích:** Nút bấm phản ứng ngay, không bị treo

### ✅ Token Blynk → `config.h`
- **Trước:** Hardcode trong `main.cpp`
- **Sau:** Tách vào `include/config.h`
- **Lợi ích:** Bảo mật tốt hơn, dễ thay đổi

### ✅ Tách code → Header files
- **Tạo:** 6 file header (buzzer, relay, alarm, buttons, commands, config)
- **Lợi ích:** Code modular, dễ bảo trì

### ✅ Lệnh Serial mới
- **Cũ:** `TR 1 ON 21:00`, `AL S 1 06:00`
- **Mới:** `R1 ON 21:00`, `A1 06:00`
- **Lợi ích:** Ngắn gọn, dễ nhớ

### ✅ Báo thức mặc định ON
- **Cũ:** `active = false` (phải bật thêm)
- **Mới:** `active = true` (mặc định bật)
- **Lợi ích:** UX tốt hơn

## 🧪 Test nhanh

### Test 1: Kiểm tra beep không-blocking
```
1. Gõ: M (Mode button)
2. Phát âm beep
3. Gõ: S (Status) → ✅ Phản ứng ngay
```

### Test 2: Test lệnh mới
```
1. Gõ: H (Help)
2. Gõ: R1 ON 21:00
3. Gõ: A1 06:00
4. Gõ: T 08:30
5. Gõ: S → ✅ Tất cả hoạt động
```

### Test 3: Báo thức mặc định ON
```
1. Gõ: A1 06:00
2. Gõ: S → ✅ ALARM 1: 06:00 [ACTIVE]
```

## 📖 Tài liệu

- **[GUIDE.md](GUIDE.md)** - Hướng dẫn chi tiết sử dụng
- **[IMPROVEMENTS.md](IMPROVEMENTS.md)** - Chi tiết cải tiến v6.0

## 🐛 Troubleshooting

### Lỗi biên dịch:
```bash
# Kiểm tra syntax
pio check

# Clean build
pio run -t clean -e nodemcuv2
pio run -e nodemcuv2
```

### Nút bấm không phản ứng:
- Kiểm tra GPIO pins trong `config.h`
- Kiểm tra INPUT_PULLUP có bật không
- Kiểm tra dây kết nối

### Blynk không kết nối:
- Kiểm tra token trong `config.h`
- Kiểm tra WiFi SSID/password
- Xem Serial monitor (9600 baud)

### Buzzer không phát âm:
- Kiểm tra GPIO buzzer pin
- Kiểm tra `silentMode` = false
- Xem phần `beep_update()` có được gọi

## 📋 Roadmap

- [ ] EEPROM: Lưu cấu hình vĩnh viễn
- [ ] Web interface: Điều khiển qua web
- [ ] OTA: Update firmware qua WiFi
- [ ] Sensor: Thêm cảm biến (nhiệt độ, ánh sáng)
- [ ] Push notification: Thông báo điện thoại
- [ ] Scene: Kích hoạt nhiều hành động

## 📝 License

MIT License - Tự do sử dụng, sửa đổi, phân phối

## 👤 Author

IoT Smart Home Project v6.0
**Cập nhật:** 23/12/2025

---

## 🎯 Quick Start

```bash
# 1. Edit config
nano include/config.h  # Set BLYNK_AUTH_TOKEN

# 2. Build & Upload
pio run -e nodemcuv2 -t upload

# 3. Monitor
pio device monitor -b 9600

# 4. Test commands
H              # See help
R1 ON 21:00   # Schedule relay
A1 06:00      # Set alarm
T 12:00       # Set time
S             # Status
```

---

**Happy Tinkering!** 🚀
