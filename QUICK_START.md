# Quick Start - ESP8266 Smart Relay Controller v7.1

## ⚡ Upload Code

```bash
platformio run -t upload -e nodemcuv2
```

## 🔌 First Boot (WiFiManager Auto-Setup)

1. Power on ESP8266
2. Phone connects to `ESP_SmartRelay` (pass: `12345678`)
3. Open browser → **192.168.4.1**
4. Select WiFi + enter password
5. Device restarts & connects ✓

## ✅ Verify Connection

Open **Serial Monitor** (9600 baud):
```
✓ WiFi Connected: [Your_SSID]
  IP: 192.168.x.x
✓ Blynk: Connecting...
```

## 📱 Blynk Setup

Create these Virtual Pins in Blynk App:

| Pin | Widget | Purpose |
|-----|--------|---------|
| V0-V3 | Button (Toggle) | Relay 1-4 control |
| V10 | Input (Text) | Send commands |
| V20 | Terminal | View feedback |

### Test Command
Send via V10: `R1 ON` → Check V20 terminal

---

## 🎮 Commands (Serial or Blynk V10)

**Relay**: `R1 ON` / `R1 OFF` / `R1` (toggle)  
**Schedule**: `SC1 ON 21:00 OFF 06:00`  
**Alarm**: `AL1 06:30`  
**Time**: `T 15:45` / `T+` (next hour)  
**Mode**: `S+` (silent on) / `TIMER+` (schedule on)  
**Reset**: `RS`  
**Status**: `S` or `1`  
**Help**: `H`

---

## 🔧 Troubleshooting

### WiFi Won't Connect
- Check SSID/password at 192.168.4.1 portal
- Hold BTN5 for 5s to reset

### Feedback Not Appearing
- Check Serial Monitor (9600 baud)
- Open Blynk V20 Terminal widget
- Should show `[Relay X] ... → ON/OFF`

### Blynk Not Connected
- Check WiFi first (Serial Monitor)
- Verify Blynk token in `config.h`
- Check internet connection

---

**Version**: 7.1 | **Status**: Production Ready ✓
