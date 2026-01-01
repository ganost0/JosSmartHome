# Tone Sound Configuration Guide

## 📍 Location: `include/config.h`

All tone durations and patterns are now **centralized in config.h** for easy customization.

---

## 🎵 Tone Duration Constants

```cpp
#define TONE_SHORT_BEEP 100       // 100ms quick beep
#define TONE_LONG_BEEP 300        // 300ms long beep
#define TONE_SHORT_DELAY 100      // 100ms delay between beeps
#define TONE_MEDIUM_DELAY 150     // 150ms medium pause
#define TONE_LONG_DELAY 200       // 200ms long pause
#define TONE_EXTRA_LONG_DELAY 350 // 350ms extra long pause
#define TONE_PAUSE_DELAY 300      // 300ms pause between patterns
```

---

## 🔊 Tone Pattern Constants

### Error Tone
```cpp
#define TONE_ERROR_COUNT 3           // 3 beeps
#define TONE_ERROR_BEEP_MS 100       // Each 100ms
#define TONE_ERROR_INTERVAL_MS 50    // 50ms between beeps
```
**Result**: 3 quick beeps (bip-bip-bip) for errors

### Mode Enter/Exit
```cpp
#define TONE_MODE_ENTER_BEEP_MS 300    // Long beep for mode enter
#define TONE_MODE_EXIT_BEEPS 2         // 2 beeps for exit
#define TONE_MODE_EXIT_FINAL_BEEP_MS 300 // Final long beep
```

### Alarm Patterns
```cpp
#define TONE_ALARM_BEEP_COUNT 3        // 3 beeps per cycle
#define TONE_ALARM_PAUSE_MS 300        // 300ms pause between cycles
#define TONE_ALARM_REPEAT_COUNT 5      // Repeat 5 times total
#define TONE_ALARM_CHIME_COUNT 2       // Hourly chime: 2 beeps
```

### Internet/System
```cpp
#define TONE_INTERNET_SOS_LONG_MS 300  // Long beep for SOS
#define TONE_INTERNET_SOS_SHORT_MS 100 // Short beep for SOS
#define TONE_SYSTEM_STARTUP_BEEP_MS 300 // System startup: long beep
```

### Click/UX
```cpp
#define TONE_CLICK_BEEP_MS 100         // Click feedback: 100ms
#define TONE_DOUBLE_CLICK_COUNT 2      // Double click: 2 beeps
```

---

## 📋 All Tone Functions & Their Patterns

| Function | Pattern | Uses Constants |
|----------|---------|---|
| `toneError()` | 3x quick beep | `TONE_ERROR_*` |
| `toneInternetDisconnect()` | SOS pattern | `TONE_INTERNET_SOS_*` |
| `toneInternetConnected()` | Short+Long beep | `TONE_SHORT_BEEP`, `TONE_LONG_BEEP` |
| `toneModeEnter()` | 1x long beep | `TONE_MODE_ENTER_*` |
| `toneModeExit()` | 2x short + 1x long | `TONE_MODE_EXIT_*` |
| `toneSystemStartup()` | 1x long beep | `TONE_SYSTEM_STARTUP_*` |
| `toneAlarmChime()` | 2x beep (hourly) | `TONE_ALARM_CHIME_COUNT` |
| `toneAlarmSleepReminder()` | Pattern x2 repeat | `TONE_LONG_BEEP`, `TONE_SHORT_BEEP` |
| `toneAlarm()` | 3x beep x5 repeat | `TONE_ALARM_*` |
| `toneClick()` | 1x click | `TONE_CLICK_BEEP_MS` |
| `toneDoubleClick()` | 2x click | `TONE_DOUBLE_CLICK_COUNT` |

---

## 🎚️ How to Customize

### Example 1: Make error tone louder (longer beeps)
```cpp
// Before
#define TONE_ERROR_BEEP_MS 100

// After
#define TONE_ERROR_BEEP_MS 150  // Longer beeps
```

### Example 2: Reduce alarm repetitions
```cpp
// Before
#define TONE_ALARM_REPEAT_COUNT 5  // 5 cycles

// After
#define TONE_ALARM_REPEAT_COUNT 3  // 3 cycles (quieter)
```

### Example 3: Speed up mode entry tone
```cpp
// Before
#define TONE_MODE_ENTER_BEEP_MS 300
#define TONE_EXTRA_LONG_DELAY 350

// After (faster)
#define TONE_MODE_ENTER_BEEP_MS 200
#define TONE_EXTRA_LONG_DELAY 250
```

### Example 4: Disable double click sound
```cpp
// Before
#define TONE_DOUBLE_CLICK_COUNT 2

// After (disable by setting to 0)
#define TONE_DOUBLE_CLICK_COUNT 0  // No double click sound
```

---

## 🔧 Implementation Details

### How Tone Functions Work

Each tone function uses:
1. `beep_start(duration_ms)` - Start buzzer for X milliseconds
2. `delay(pause_ms)` - Wait before next beep
3. `beep_update()` - Update buzzer state

Example from `toneError()`:
```cpp
void toneError() {
  for (int i = 0; i < TONE_ERROR_COUNT; i++) {      // 3 times
    beep_start(TONE_ERROR_BEEP_MS);                  // 100ms beep
    delay(TONE_SHORT_DELAY);                         // 100ms delay
    beep_update();                                   // Update state
    delay(TONE_ERROR_INTERVAL_MS);                   // 50ms pause
  }
}
```

---

## 🎯 Tone Usage in Code

### Feedback Tones
```cpp
toneClick();           // Success feedback (command executed)
toneError();           // Error feedback (invalid command)
toneDoubleClick();     // Confirmation feedback
```

### Event Tones
```cpp
toneAlarm();           // Alarm triggered
toneAlarmChime();      // Hourly chime (6 AM, 12 PM, etc.)
toneAlarmSleepReminder(); // Before alarm (prep)
```

### System Tones
```cpp
toneSystemStartup();   // On reset/restart
toneInternetConnected(); // WiFi/Blynk connected
toneInternetDisconnect(); // WiFi/Blynk disconnected
```

### Mode Tones
```cpp
toneModeEnter();       // Enter special mode (S+, TIMER+)
toneModeExit();        // Exit special mode (S-, TIMER-)
```

---

## 📊 Timing Reference

| Constant | Value | Use Case |
|----------|-------|----------|
| `TONE_SHORT_BEEP` | 100ms | Quick feedback |
| `TONE_LONG_BEEP` | 300ms | Emphasis/warning |
| `TONE_SHORT_DELAY` | 100ms | Between quick beeps |
| `TONE_MEDIUM_DELAY` | 150ms | Comfortable pause |
| `TONE_LONG_DELAY` | 200ms | Extended pause |
| `TONE_EXTRA_LONG_DELAY` | 350ms | Clear separation |
| `TONE_PAUSE_DELAY` | 300ms | Between patterns |

---

## 🚀 Quick Customization Guide

**Want quieter?** → Reduce `TONE_LONG_BEEP` or `TONE_REPEAT_COUNT`  
**Want faster?** → Reduce all DELAY values  
**Want more distinctive?** → Increase duration contrasts  
**Want silent?** → Set counts to 0 or durations to 0  

---

## ✅ Benefits of Config-Based Tones

✓ **Easy to customize** - No code recompilation needed (just edit config.h)  
✓ **Centralized** - All tone settings in one file  
✓ **Consistent** - Uses same constants across all functions  
✓ **Maintainable** - Easy to find and modify tone behavior  
✓ **Professional** - Can adjust sounds for different regions/preferences  

---

## 📝 Version

**Added in**: v7.1  
**Location**: `include/config.h` (lines XX-YY)  
**Status**: Production Ready ✓

