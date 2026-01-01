# Smart Relay Controller v7.1 - Feedback Synchronization Guide

## 📡 Feedback Synchronization (v7.1 Update)

### Overview
Tất cả các feedback từ lệnh (Serial, Button, Blynk) đều được đồng bộ hóa để hiển thị **giống nhau** trên cả:
- 📱 Serial Monitor (Terminal)
- 📲 Blynk App (V10, V20)
- 🔘 Physical Buttons (tones)

---

## 🔧 Implementation

### Unified Feedback Function

```cpp
// Gửi feedback đến cả Serial và Blynk
void sendFeedback(const char* message) {
  // In ra Serial Monitor
  Serial.println(message);
  
  // Gửi tới Blynk V20 (Terminal widget)
  if (wifiConnected && Blynk.connected()) {
    Blynk.virtualWrite(V20, message);
  }
}

// Version với format string
void sendFeedbackf(const char* format, ...) {
  char buffer[256];
  va_list args;
  va_start(args, format);
  vsnprintf(buffer, sizeof(buffer), format, args);
  va_end(args);
  sendFeedback(buffer);
}
```

---

## 📊 Feedback Flow

### When Command from Serial:
```
Serial Input (Terminal)
         ↓
   processSerialCommand()
         ↓
   sendFeedback() ← Sync point
         ↓
   ┌─────┴─────┐
   ↓           ↓
Serial Out   Blynk V20
(Terminal)   (App)
```

### When Command from Blynk V11:
```
Blynk Input (App)
      ↓
BLYNK_WRITE(V11)
      ↓
processSerialCommand()
      ↓
sendFeedback() ← Sync point
      ↓
┌─────┴──────┐
↓            ↓
Serial Out   Blynk V20
(Terminal)   (App)
```

### When Command from Button:
```
Button Press
      ↓
handleButtons()
      ↓
setRelay() → sendFeedback()
      ↓
┌─────┴──────┐
↓            ↓
Serial Out   Blynk V20
(Terminal)   (App)
```

---

## 🎯 Feedback Examples

### Relay Control
**Command**: `R1 ON`

**Output on Serial**:
```
[Relay 1] LIGHT → ON
```

**Output on Blynk V20**:
```
[Relay 1] LIGHT → ON
```

---

### Schedule Setting
**Command**: `SC1 ON 21:00 OFF 06:00`

**Output on Serial**:
```
[Schedule R1 (LIGHT)] ON 21:00 → OFF 06:00
```

**Output on Blynk V20**:
```
[Schedule R1 (LIGHT)] ON 21:00 → OFF 06:00
```

---

### Alarm Setting
**Command**: `AL1 06:30`

**Output on Serial**:
```
[Alarm 1] Set to 06:30
```

**Output on Blynk V20**:
```
[Alarm 1] Set to 06:30
```

---

### Time Setting
**Command**: `T 15:45`

**Output on Serial**:
```
[Time] Set to 15:45
```

**Output on Blynk V20**:
```
[Time] Set to 15:45
```

---

### Mode Changes
**Command**: `S+` (Silent mode ON)

**Output on Serial**:
```
[Silent] Mode: ON
```

**Output on Blynk V20**:
```
[Silent] Mode: ON
```

---

### System Reset
**Command**: `RS`

**Output on Serial**:
```
[Reset] All systems reset
```

**Output on Blynk V20**:
```
[Reset] All systems reset
```

---

### Error Handling
**Command**: `SC5 ON 21:00 OFF 06:00` (Invalid relay)

**Output on Serial**:
```
[Command] Unknown: SC5 ON 21:00 OFF 06:00
```

**Output on Blynk V20**:
```
[Command] Unknown: SC5 ON 21:00 OFF 06:00
```

---

## 🔊 Tone Feedback (Synchronized)

Tất cả lệnh cũng phát âm thanh tương ứng:

| Event | Tone | Serial | Blynk |
|-------|------|--------|-------|
| Success | Click | ✓ | ✓ |
| Error | Error (3x) | ✓ | ✓ |
| Mode Enter | Long beep | ✓ | ✓ |
| Mode Exit | 3x beep | ✓ | ✓ |
| Reset | System | ✓ | ✓ |

---

## 📱 Blynk Configuration

### V10: Command Feedback (Single Response)
```
Widget: Value Display / Terminal
Update: After command processing
Source: BLYNK_WRITE(V10) handler
```

### V11: Command Input
```
Widget: Input Box / Styled Button
Update: When user sends command
Action: Calls processSerialCommand()
```

### V20: Status Terminal (Continuous Feedback)
```
Widget: Terminal
Update: All sendFeedback() calls
Source: Unified feedback function
```

---

## 💾 Key Code Changes (v7.1)

### Added Functions:
```cpp
void sendFeedback(const char* message);      // Basic feedback
void sendFeedbackf(const char* format, ...); // Format string version
```

### Updated Functions:
- `setRelay()` - Uses `sendFeedbackf()`
- `processSerialCommand()` - All Serial.println → `sendFeedback()`
- `BLYNK_WRITE(V10)` - Uses `sendFeedback()`
- `BLYNK_WRITE(V11)` - Uses `sendFeedbackf()`

### Affected Commands:
- R1-R4 ON/OFF - ✅ Synced
- SC1-SC4 schedules - ✅ Synced
- AL1-AL2 alarms - ✅ Synced
- T time setting - ✅ Synced
- S+/S- silent mode - ✅ Synced
- TIMER+/TIMER- - ✅ Synced
- RS reset - ✅ Synced
- Error messages - ✅ Synced

---

## 🧪 Testing

### Test on Serial:
```bash
platformio run -t monitor -e nodemcuv2
# Send: R1 ON
# Expect: [Relay 1] LIGHT → ON
```

### Test on Blynk V11:
1. Open Blynk App
2. Send: `R1 ON` via V11 input
3. Check V20 terminal for feedback
4. Check Serial Monitor for same message

### Test from Button:
1. Press physical button (e.g., BTN1)
2. Check Serial Monitor for feedback
3. Check Blynk V20 for same message

---

## 📋 Status Matrix

| Source | Serial Monitor | Blynk V20 | Tone |
|--------|---|---|---|
| Serial Command | ✅ | ✅ | ✅ |
| Blynk V11 | ✅ | ✅ | ✅ |
| Physical Button | ✅ | ✅ | ✅ |
| Timer/Alarm | ✅ | ✅ | ✅ |

---

## 🔗 Integration Points

### Serial → Blynk
```
Serial Input
  ↓
processSerialCommand()
  ↓
sendFeedback() [SYNC]
  ↓
Serial Monitor + Blynk V20
```

### Blynk → Serial
```
Blynk V11 Input
  ↓
BLYNK_WRITE(V11)
  ↓
processSerialCommand()
  ↓
sendFeedback() [SYNC]
  ↓
Serial Monitor + Blynk V20
```

### Button → All
```
Button Press
  ↓
handleButtons()
  ↓
setRelay()
  ↓
sendFeedback() [SYNC]
  ↓
Serial Monitor + Blynk V20 + Tone
```

---

## 🎓 Usage Example

### Scenario: Control Light from Blynk
```
1. Open Blynk App
2. Press button in V11 (Input)
3. Type: R1 ON
4. Send

Result:
- Serial Monitor prints: [Relay 1] LIGHT → ON
- Blynk V20 shows: [Relay 1] LIGHT → ON
- Buzzer plays: Click tone
- Relay 1 turns ON
```

### Scenario: Control Light from Serial
```
1. Open Serial Monitor (9600 baud)
2. Type: R1 ON
3. Send (press Enter)

Result:
- Serial Monitor prints: [Relay 1] LIGHT → ON
- Blynk V20 shows: [Relay 1] LIGHT → ON
- Buzzer plays: Click tone
- Relay 1 turns ON
```

---

## ✅ Verification Checklist

- [ ] Relay commands appear on Serial AND Blynk V20
- [ ] Schedule commands appear on Serial AND Blynk V20
- [ ] Alarm commands appear on Serial AND Blynk V20
- [ ] Time commands appear on Serial AND Blynk V20
- [ ] Mode commands appear on Serial AND Blynk V20
- [ ] Error messages appear on Serial AND Blynk V20
- [ ] Tone feedback plays for all commands
- [ ] Dashboard updates after each command
- [ ] Button commands sync with Serial/Blynk

---

## 🚀 Deployment

This synchronization is automatically enabled in v7.1:
```bash
platformio run -t upload -e nodemcuv2
```

No additional configuration needed. Just ensure:
1. Blynk token is set in `config.h`
2. WiFi is configured via WiFiManager
3. Serial Monitor is at 9600 baud

---

**Version**: 7.1  
**Status**: Production Ready ✓  
**Last Updated**: Jan 2026
