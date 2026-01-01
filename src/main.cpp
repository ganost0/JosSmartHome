#define BLYNK_TEMPLATE_ID "TMPLxxxxxx"
#define BLYNK_TEMPLATE_NAME "Smart Home Control"
#define BLYNK_PRINT Serial

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <WiFiManager.h>
#include <time.h>
#include "config.h"

// ===== GPIO MAPPING =====
const uint8_t RELAY_PINS[4] = {D1, D2, D4, D6};
const uint8_t BUZZER_PIN = D5;
const uint8_t WIFI_LED = D0;

// ===== BUTTON PINS (5 buttons) =====
// D3(GPIO-0):   Button 1 - Control Relay 1
// SD3(GPIO-10): Button 2 - Control Relay 2  
// D7(GPIO-13):  Button 3 - Control Relay 3
// RX(GPIO-3):   Button 4 - Control Relay 4
// D8(GPIO-15):  Button 5 - Reset (hold 5s)
const uint8_t BUTTON_PINS[5] = {D3, 10, D7, 3, D8};  // GPIO pins
const uint8_t BUTTON_GPIO[5] = {0, 10, 13, 3, 15};   // Actual GPIO numbers
const char* BUTTON_NAMES[5] = {"BTN1(R1)", "BTN2(R2)", "BTN3(R3)", "BTN4(R4)", "BTN5(RST)"};
unsigned long buttonPressTimes[5] = {0, 0, 0, 0, 0};
bool buttonPressed[5] = {false, false, false, false, false};

// ===== BLYNK (Token from config.h) =====
char auth[] = BLYNK_AUTH_TOKEN;
BlynkTimer timer;

// ===== CONSTANTS =====
#define MODE_NORMAL 0
#define MODE_CONTROL 1
#define MODE_CHIMING 2

// ===== STATE =====
bool relayState[4] = {false, false, false, false};
bool timerControlEnabled = true;
bool chimingEnabled = true;
bool silentMode = false;
bool buzzing = false;
unsigned long buzzStart = 0;
unsigned long buzzDuration = 0;
int currentMode = MODE_NORMAL;
int currentHour = 0, currentMin = 0, currentSec = 0;
unsigned long lastUpdate = 0;
bool wifiConnected = false;
String wifiName = "";
String lastAction = "---";
String lastError = "---";
bool toBlynk = true;

struct {
  int onHour = -1, onMin = 0;
  int offHour = -1, offMin = 0;
} relaySchedules[4];

struct {
  int hour = -1, min = 0;
  bool active = true;
} alarms[2];

const char* relayNames[4] = {"LIGHT", "FAN", "PUMP", "TV"};

// ===== BUZZER =====
void beep_start(unsigned long duration) {
  digitalWrite(BUZZER_PIN, HIGH);
  buzzing = true;
  buzzStart = millis();
  buzzDuration = duration;
}

void beep_update() {
  if (!buzzing) return;
  if (millis() - buzzStart >= buzzDuration) {
    digitalWrite(BUZZER_PIN, LOW);
    buzzing = false;
  }
}

// ===== TONE SOUNDS =====
// ERROR: Bip x3 (nhanh)
void toneError() {
  for (int i = 0; i < TONE_ERROR_COUNT; i++) {
    beep_start(TONE_ERROR_BEEP_MS);
    delay(TONE_SHORT_DELAY);
    beep_update();
    delay(TONE_ERROR_INTERVAL_MS);
  }
}

// INTERNET Disconnect: SOS (bip dài – bip – bip dài)
void toneInternetDisconnect() {
  beep_start(TONE_INTERNET_SOS_LONG_MS);
  delay(TONE_EXTRA_LONG_DELAY);
  beep_update();
  beep_start(TONE_INTERNET_SOS_SHORT_MS);
  delay(TONE_MEDIUM_DELAY);
  beep_update();
  beep_start(TONE_INTERNET_SOS_SHORT_MS);
  delay(TONE_MEDIUM_DELAY);
  beep_update();
  beep_start(TONE_INTERNET_SOS_LONG_MS);
  delay(TONE_EXTRA_LONG_DELAY);
  beep_update();
}

// INTERNET Connected: Bip ngắn – bip dài
void toneInternetConnected() {
  beep_start(TONE_SHORT_BEEP);
  delay(TONE_MEDIUM_DELAY);
  beep_update();
  beep_start(TONE_LONG_BEEP);
  delay(TONE_EXTRA_LONG_DELAY);
  beep_update();
}

// MODE Enter: Bip dài
void toneModeEnter() {
  beep_start(TONE_MODE_ENTER_BEEP_MS);
  delay(TONE_EXTRA_LONG_DELAY);
  beep_update();
}

// MODE Out: Bip – bip – bip dài
void toneModeExit() {
  for (int i = 0; i < TONE_MODE_EXIT_BEEPS; i++) {
    beep_start(TONE_SHORT_BEEP);
    delay(TONE_MEDIUM_DELAY);
    beep_update();
  }
  beep_start(TONE_MODE_EXIT_FINAL_BEEP_MS);
  delay(TONE_EXTRA_LONG_DELAY);
  beep_update();
}

// SYSTEM Startup/Reset: Bip dài x1
void toneSystemStartup() {
  beep_start(TONE_SYSTEM_STARTUP_BEEP_MS);
  delay(TONE_EXTRA_LONG_DELAY);
  beep_update();
}

// ALARM Hour chime: Bip x2
void toneAlarmChime() {
  for (int i = 0; i < TONE_ALARM_CHIME_COUNT; i++) {
    beep_start(TONE_SHORT_BEEP);
    delay(TONE_MEDIUM_DELAY);
    beep_update();
  }
}

// ALARM Sleep reminder: Bip dài – bip – bip (x2)
void toneAlarmSleepReminder() {
  for (int rep = 0; rep < 2; rep++) {
    beep_start(TONE_LONG_BEEP);
    delay(TONE_EXTRA_LONG_DELAY);
    beep_update();
    beep_start(TONE_SHORT_BEEP);
    delay(TONE_MEDIUM_DELAY);
    beep_update();
    beep_start(TONE_SHORT_BEEP);
    delay(TONE_MEDIUM_DELAY);
    beep_update();
    delay(TONE_LONG_DELAY);
  }
}

// ALARM Alarm: (bip x3 – pause) x5
void toneAlarm() {
  for (int rep = 0; rep < TONE_ALARM_REPEAT_COUNT; rep++) {
    for (int i = 0; i < TONE_ALARM_BEEP_COUNT; i++) {
      beep_start(TONE_SHORT_BEEP);
      delay(TONE_SHORT_DELAY);
      beep_update();
      delay(TONE_ERROR_INTERVAL_MS);
    }
    delay(TONE_ALARM_PAUSE_MS);
  }
}

// UX Click: Bip
void toneClick() {
  beep_start(TONE_CLICK_BEEP_MS);
  delay(TONE_MEDIUM_DELAY);
  beep_update();
}

// UX Double click: Bip bip
void toneDoubleClick() {
  for (int i = 0; i < TONE_DOUBLE_CLICK_COUNT; i++) {
    beep_start(TONE_SHORT_BEEP);
    delay(TONE_SHORT_DELAY);
    beep_update();
    delay(TONE_ERROR_INTERVAL_MS);
  }
}

// Legacy functions for backward compatibility
void toneOk() { toneClick(); }
void toneSuccess() { toneAlarmChime(); }

// ===== TIME & NTP =====
void updateTime() {
  if (millis() - lastUpdate < 1000) return;
  lastUpdate = millis();
  // Time is manually set via serial command (T HH:MM)
  // No NTP sync - just increment seconds for real-time simulation
  currentSec++;
  if (currentSec >= 60) {
    currentSec = 0;
    currentMin++;
    if (currentMin >= 60) {
      currentMin = 0;
      currentHour++;
      if (currentHour >= 24) {
        currentHour = 0;
      }
    }
  }
}

// ===== UNIFIED FEEDBACK (Serial + Blynk) =====
void sendFeedback(const char* message) {
  // Send to Serial
  Serial.println(message);
  
  // Send to Blynk V20 (Terminal widget) if connected
  if (wifiConnected && Blynk.connected()) {
    Blynk.virtualWrite(V20, message);
  }
}

void sendFeedbackf(const char* format, ...) {
  char buffer[256];
  va_list args;
  va_start(args, format);
  vsnprintf(buffer, sizeof(buffer), format, args);
  va_end(args);
  sendFeedback(buffer);
}

// ===== RELAY =====
void setRelay(int index, bool state) {
  if (index < 0 || index >= 4) return;
  relayState[index] = state;
  digitalWrite(RELAY_PINS[index], state ? HIGH : LOW);
  toneClick();
  sendFeedbackf("[Relay %d] %s → %s", index + 1, relayNames[index], state ? "ON" : "OFF");
}

// ===== ALARM & SCHEDULE =====
void checkAlarms() {
  for (int i = 0; i < 2; i++) {
    if (!alarms[i].active || alarms[i].hour < 0) continue;
    if (currentHour == alarms[i].hour && currentMin == alarms[i].min && currentSec == 0) {
      Serial.printf("\n🔔 ALARM %d TRIGGERED! (%02d:%02d)\n", i + 1, alarms[i].hour, alarms[i].min);
      if (!silentMode) {
        for (int j = 0; j < 5; j++) {
          toneSuccess();
          delay(200);
          beep_update();
          delay(100);
        }
      }
    }
  }
}

void checkSchedules() {
  for (int i = 0; i < 4; i++) {
    if (!timerControlEnabled) continue;
    
    // Check ON time
    if (relaySchedules[i].onHour >= 0) {
      if (currentHour == relaySchedules[i].onHour && currentMin == relaySchedules[i].onMin && currentSec == 0) {
        if (!relayState[i]) {
          setRelay(i, true);
          Serial.printf("⏰ Schedule: %s turned ON\n", relayNames[i]);
        }
      }
    }
    
    // Check OFF time
    if (relaySchedules[i].offHour >= 0) {
      if (currentHour == relaySchedules[i].offHour && currentMin == relaySchedules[i].offMin && currentSec == 0) {
        if (relayState[i]) {
          setRelay(i, false);
          Serial.printf("⏰ Schedule: %s turned OFF\n", relayNames[i]);
        }
      }
    }
  }
}

// ===== BUTTON HANDLING =====
void handleButtons() {
  for (int i = 0; i < 5; i++) {
    bool isPressed = (digitalRead(BUTTON_PINS[i]) == LOW);
    
    // Button press detected
    if (isPressed && !buttonPressed[i]) {
      buttonPressed[i] = true;
      buttonPressTimes[i] = millis();
      Serial.printf("[%s] Pressed\n", BUTTON_NAMES[i]);
    }
    
    // Button release detected
    if (!isPressed && buttonPressed[i]) {
      unsigned long pressDuration = millis() - buttonPressTimes[i];
      buttonPressed[i] = false;
      
      if (i < 4) {
        // Buttons 1-4: Toggle relay
        setRelay(i, !relayState[i]);
        Serial.printf("[%s] Toggled: %s\n", BUTTON_NAMES[i], relayState[i] ? "ON" : "OFF");
        toneOk();
      } else {
        // Button 5: Reset if held >= 5 seconds
        if (pressDuration >= 5000) {
          Serial.println("[BTN5] Reset triggered (held 5s)");
          for (int j = 0; j < 4; j++) setRelay(j, false);
          timerControlEnabled = true;
          silentMode = false;
          currentMode = MODE_NORMAL;
          for (int j = 0; j < 3; j++) {
            toneSuccess();
            delay(150);
            beep_update();
            delay(100);
          }
          Serial.println("✓ All reset!");
        }
      }
    }
  }
}

// ===== DASHBOARD =====
void printDashboard() {
  char timeStr[6];
  sprintf(timeStr, "%02d:%02d", currentHour, currentMin);
  
  Serial.println("\n╔═══════════════════════════════════════════════════════════════╗");
  Serial.printf("║ ⏰ TIME: %s                WiFi: %s %-25s ║\n", 
                timeStr,
                wifiConnected ? "✓" : "✗",
                wifiConnected ? wifiName.c_str() : "Disconnected");
  Serial.println("║───────────────────────────────────────────────────────────────║");
  Serial.printf("║ Mode: %-8s              Timer: %s                     ║\n",
                currentMode == MODE_NORMAL ? "NORMAL" : (currentMode == MODE_CONTROL ? "CONTROL" : "CHIMING"),
                timerControlEnabled ? "✓ON" : "✗OFF");
  Serial.printf("║ Silent: %-6s             Chiming: %s                   ║\n",
                silentMode ? "✓ON" : "✗OFF",
                chimingEnabled ? "✓ON" : "✗OFF");
  Serial.println("║───────────────────────────────────────────────────────────────║");
  
  Serial.printf("║ 💡 RELAYS:                                                     ║\n");
  Serial.printf("║   R1 (LIGHT): %s          R2 (FAN):   %s                   ║\n",
                relayState[0] ? "✓ON " : "✗OFF",
                relayState[1] ? "✓ON " : "✗OFF");
  Serial.printf("║   R3 (PUMP):  %s          R4 (TV):    %s                   ║\n",
                relayState[2] ? "✓ON " : "✗OFF",
                relayState[3] ? "✓ON " : "✗OFF");
  Serial.println("║───────────────────────────────────────────────────────────────║");
  
  Serial.printf("║ 📅 SCHEDULES:                                                  ║\n");
  Serial.print("║   R1(LIGHT): ");
  if (relaySchedules[0].onHour >= 0) {
    Serial.printf("ON %02d:%02d → OFF %02d:%02d    ",
                  relaySchedules[0].onHour, relaySchedules[0].onMin,
                  relaySchedules[0].offHour, relaySchedules[0].offMin);
  } else {
    Serial.print("-- : -- → -- : --        ");
  }
  Serial.println("║");
  
  Serial.print("║   R2(FAN):   ");
  if (relaySchedules[1].onHour >= 0 || relaySchedules[1].offHour >= 0) {
    if (relaySchedules[1].onHour >= 0) {
      Serial.printf("ON %02d:%02d ", relaySchedules[1].onHour, relaySchedules[1].onMin);
    }
    if (relaySchedules[1].offHour >= 0) {
      Serial.printf("→ OFF %02d:%02d ", relaySchedules[1].offHour, relaySchedules[1].offMin);
    }
    Serial.print("                   ");
  } else {
    Serial.print("-- : -- → -- : --                   ");
  }
  Serial.println("║");
  
  Serial.print("║   R3(PUMP):  ");
  if (relaySchedules[2].onHour >= 0 || relaySchedules[2].offHour >= 0) {
    if (relaySchedules[2].onHour >= 0) {
      Serial.printf("ON %02d:%02d ", relaySchedules[2].onHour, relaySchedules[2].onMin);
    }
    if (relaySchedules[2].offHour >= 0) {
      Serial.printf("→ OFF %02d:%02d ", relaySchedules[2].offHour, relaySchedules[2].offMin);
    }
    Serial.print("                   ");
  } else {
    Serial.print("-- : -- → -- : --                   ");
  }
  Serial.println("║");
  
  Serial.print("║   R4(TV):    ");
  if (relaySchedules[3].onHour >= 0 || relaySchedules[3].offHour >= 0) {
    if (relaySchedules[3].onHour >= 0) {
      Serial.printf("ON %02d:%02d ", relaySchedules[3].onHour, relaySchedules[3].onMin);
    }
    if (relaySchedules[3].offHour >= 0) {
      Serial.printf("→ OFF %02d:%02d ", relaySchedules[3].offHour, relaySchedules[3].offMin);
    }
    Serial.print("                   ");
  } else {
    Serial.print("-- : -- → -- : --                   ");
  }
  Serial.println("║");
  
  Serial.println("║───────────────────────────────────────────────────────────────║");
  Serial.print("║ 🔔 ALARMS: A1: ");
  if (alarms[0].hour >= 0) {
    Serial.printf("%02d:%02d [%s]          A2: ",
                  alarms[0].hour, alarms[0].min,
                  alarms[0].active ? "✓ON" : "✗OFF");
  } else {
    Serial.print("-- : -- [✗OFF]          A2: ");
  }
  
  if (alarms[1].hour >= 0) {
    Serial.printf("%02d:%02d [%s]    ║\n",
                  alarms[1].hour, alarms[1].min,
                  alarms[1].active ? "✓ON" : "✗OFF");
  } else {
    Serial.println("-- : -- [✗OFF]    ║");
  }
  
  Serial.println("║───────────────────────────────────────────────────────────────║");
  Serial.println("║ 🔘 BUTTONS:                                                    ║");
  Serial.println("║   BTN1(GPIO-0):   Toggle R1(LIGHT)  | BTN2(GPIO-10): Toggle R2 ║");
  Serial.println("║   BTN3(GPIO-13):  Toggle R3(PUMP)   | BTN4(GPIO-3):  Toggle R4  ║");
  Serial.println("║   BTN5(GPIO-15):  Reset (hold 5s)                              ║");
  Serial.println("╚═══════════════════════════════════════════════════════════════╝\n");
}

// ===== SERIAL COMMANDS =====
// Schedule format: SC1 ON 21:00 OFF 06:00 (set schedule for relay 1)
//                  SC1 OFF (disable schedule for relay 1)
// Alarm format:    AL1 06:30 (set alarm 1 to 06:30)
//                  AL1 OFF (disable alarm 1)
void processSerialCommand(String cmd) {
  cmd.trim();
  cmd.toUpperCase();
  if (cmd.length() == 0) return;
  
  // SC1 ON 21:00 OFF 06:00 - Schedule for relay
  if (cmd.startsWith("SC") && cmd.length() >= 3) {
    int num = cmd[2] - '0' - 1;
    if (num >= 0 && num < 4) {
      if (cmd.indexOf("OFF") == 0 || cmd.indexOf("OFF") > 0) {
        // Disable schedule
        relaySchedules[num].onHour = -1;
        relaySchedules[num].offHour = -1;
        sendFeedbackf("[Schedule R%d] Disabled", num + 1);
        toneClick();
      } else if (cmd.indexOf("ON") > 0 && cmd.indexOf("OFF") > cmd.indexOf("ON")) {
        // Parse: SC1 ON 21:00 OFF 06:00
        int onPos = cmd.indexOf("ON");
        int offPos = cmd.indexOf("OFF");
        
        String onTimeStr = cmd.substring(onPos + 2, offPos);
        String offTimeStr = cmd.substring(offPos + 3);
        
        onTimeStr.trim();
        offTimeStr.trim();
        
        int colonPos1 = onTimeStr.indexOf(':');
        int colonPos2 = offTimeStr.indexOf(':');
        
        if (colonPos1 > 0 && colonPos2 > 0) {
          int onHour = onTimeStr.substring(0, colonPos1).toInt();
          int onMin = onTimeStr.substring(colonPos1 + 1).toInt();
          int offHour = offTimeStr.substring(0, colonPos2).toInt();
          int offMin = offTimeStr.substring(colonPos2 + 1).toInt();
          
          if (onHour >= 0 && onHour < 24 && onMin >= 0 && onMin < 60 &&
              offHour >= 0 && offHour < 24 && offMin >= 0 && offMin < 60) {
            relaySchedules[num].onHour = onHour;
            relaySchedules[num].onMin = onMin;
            relaySchedules[num].offHour = offHour;
            relaySchedules[num].offMin = offMin;
            sendFeedbackf("[Schedule R%d (%s)] ON %02d:%02d → OFF %02d:%02d", 
                         num + 1, relayNames[num], onHour, onMin, offHour, offMin);
            toneClick();
          } else {
            sendFeedback("[Schedule] Invalid time format");
            toneError();
          }
        }
      }
      printDashboard();
    }
    return;
  }
  
  // AL1 06:30 - Set alarm
  if (cmd.startsWith("AL") && cmd.length() >= 3) {
    int num = cmd[2] - '0' - 1;
    if (num >= 0 && num < 2) {
      if (cmd.indexOf("OFF") > 0) {
        alarms[num].active = false;
        sendFeedbackf("[Alarm %d] Disabled", num + 1);
        toneClick();
      } else if (cmd.indexOf(':') > 0) {
        int pos = cmd.indexOf(':');
        int h = cmd.substring(3, pos).toInt();
        int m = cmd.substring(pos + 1).toInt();
        if (h >= 0 && h < 24 && m >= 0 && m < 60) {
          alarms[num].hour = h;
          alarms[num].min = m;
          alarms[num].active = true;
          sendFeedbackf("[Alarm %d] Set to %02d:%02d", num + 1, h, m);
          toneClick();
        } else {
          sendFeedback("[Alarm] Invalid time format");
          toneError();
        }
      }
      printDashboard();
    }
    return;
  }
  
  // R1 ON/OFF / R1
  if (cmd[0] == 'R' && cmd.length() >= 2) {
    int num = cmd[1] - '0' - 1;
    if (num >= 0 && num < 4) {
      if (cmd.indexOf("ON") > 0) {
        setRelay(num, true);
      }
      else if (cmd.indexOf("OFF") > 0) {
        setRelay(num, false);
      } else if (cmd.length() == 2) {
        // Just "R1" - toggle
        setRelay(num, !relayState[num]);
      }
      printDashboard();
    }
    return;
  }
  
  // A1 12:30 or A1 OFF (legacy - use AL1 instead)
  if (cmd[0] == 'A' && cmd.length() >= 2 && !cmd.startsWith("AL")) {
    int num = cmd[1] - '0' - 1;
    if (num >= 0 && num < 2) {
      if (cmd.indexOf("OFF") > 0) {
        alarms[num].active = false;
        Serial.printf("[Alarm %d] Disabled\n", num + 1);
      } else if (cmd.indexOf(':') > 0) {
        int pos = cmd.indexOf(':');
        alarms[num].hour = cmd.substring(2, pos).toInt();
        alarms[num].min = cmd.substring(pos + 1).toInt();
        alarms[num].active = true;
        Serial.printf("[Alarm %d] Set to %02d:%02d\n", num + 1, alarms[num].hour, alarms[num].min);
      }
      printDashboard();
    }
    return;
  }
  
  // T HH:MM or T+
  if (cmd[0] == 'T') {
    if (cmd == "T+") {
      currentHour = (currentHour + 1) % 24;
      sendFeedbackf("[Time] Advanced to %02d:00", currentHour);
    } else if (cmd.indexOf(':') > 0) {
      int pos = cmd.indexOf(':');
      int h = cmd.substring(1, pos).toInt();
      int m = cmd.substring(pos + 1).toInt();
      if (h >= 0 && h < 24 && m >= 0 && m < 60) {
        currentHour = h;
        currentMin = m;
        currentSec = 0;
        sendFeedbackf("[Time] Set to %02d:%02d", currentHour, currentMin);
        toneClick();
      } else {
        sendFeedback("[Time] Invalid time format");
        toneError();
      }
    }
    printDashboard();
    return;
  }
  
  // S+/- Silent
  if (cmd == "S+") {
    silentMode = true;
    sendFeedback("[Silent] Mode: ON");
    toneModeEnter();
    printDashboard();
    return;
  }
  if (cmd == "S-") {
    silentMode = false;
    sendFeedback("[Silent] Mode: OFF");
    toneModeExit();
    printDashboard();
    return;
  }
  
  // TIMER+/- Timer control
  if (cmd == "TIMER+") {
    timerControlEnabled = true;
    sendFeedback("[Timer] Control: ON");
    toneModeEnter();
    printDashboard();
    return;
  }
  if (cmd == "TIMER-") {
    timerControlEnabled = false;
    sendFeedback("[Timer] Control: OFF");
    toneModeExit();
    printDashboard();
    return;
  }
  
  // RS Reset
  if (cmd == "RS") {
    for (int i = 0; i < 4; i++) setRelay(i, false);
    timerControlEnabled = true;
    silentMode = false;
    currentMode = MODE_NORMAL;
    sendFeedback("[Reset] All systems reset");
    toneSystemStartup();
    printDashboard();
    return;
  }
  
  // Test commands
  if (cmd == "TESTOK") { toneClick(); return; }
  if (cmd == "TESTERR") { toneError(); return; }
  if (cmd == "TESTALARM") { toneAlarm(); return; }
  if (cmd == "TESTHOUR") { currentHour = 12; currentMin = 0; printDashboard(); return; }
  if (cmd == "TESTSLEEP") { currentHour = 23; currentMin = 0; printDashboard(); return; }
  
  // Status & Help
  if (cmd == "S" || cmd == "1") {
    printDashboard();
    return;
  }
  
  if (cmd == "H") {
    Serial.println("\n╔════════════════════════════════════════════════════╗");
    Serial.println("║         SMART RELAY COMMANDS v7.1 (Updated)       ║");
    Serial.println("╠════════════════════════════════════════════════════╣");
    Serial.println("║  RELAY CONTROL:                                    ║");
    Serial.println("║    R1 ON / R1 OFF   - Control Relay 1 (LIGHT)      ║");
    Serial.println("║    R2 ON / R2 OFF   - Control Relay 2 (FAN)        ║");
    Serial.println("║    R3 ON / R3 OFF   - Control Relay 3 (PUMP)       ║");
    Serial.println("║    R4 ON / R4 OFF   - Control Relay 4 (TV)         ║");
    Serial.println("║    R1 / R2 / R3 / R4 - Toggle relay                ║");
    Serial.println("╠════════════════════════════════════════════════════╣");
    Serial.println("║  SCHEDULE (Each relay ON -> OFF time):             ║");
    Serial.println("║    SC1 ON 21:00 OFF 06:00  - R1 schedule           ║");
    Serial.println("║    SC2 ON 19:00 OFF 23:00  - R2 schedule           ║");
    Serial.println("║    SC3 ON 06:00 OFF 18:00  - R3 schedule           ║");
    Serial.println("║    SC4 ON 20:00 OFF 22:00  - R4 schedule           ║");
    Serial.println("║    SC1 OFF                 - Disable R1 schedule   ║");
    Serial.println("╠════════════════════════════════════════════════════╣");
    Serial.println("║  ALARM:                                            ║");
    Serial.println("║    AL1 06:00        - Set Alarm 1 to 06:00         ║");
    Serial.println("║    AL2 12:30        - Set Alarm 2 to 12:30         ║");
    Serial.println("║    AL1 OFF          - Disable Alarm 1              ║");
    Serial.println("║    A1 06:00         - Legacy (same as AL1)         ║");
    Serial.println("╠════════════════════════════════════════════════════╣");
    Serial.println("║  TIME:                                             ║");
    Serial.println("║    T 15:45          - Set time to 15:45            ║");
    Serial.println("║    T+               - Time +1 hour                 ║");
    Serial.println("╠════════════════════════════════════════════════════╣");
    Serial.println("║  MODE:                                             ║");
    Serial.println("║    S+               - Silent mode ON               ║");
    Serial.println("║    S-               - Silent mode OFF              ║");
    Serial.println("║    TIMER+/TIMER-    - Enable/Disable schedule      ║");
    Serial.println("║    RS               - Reset all                    ║");
    Serial.println("╠════════════════════════════════════════════════════╣");
    Serial.println("║  STATUS & HELP:                                    ║");
    Serial.println("║    S  or  1         - Show status                  ║");
    Serial.println("║    H                - Show this help               ║");
    Serial.println("╠════════════════════════════════════════════════════╣");
    Serial.println("║  TEST COMMANDS:                                    ║");
    Serial.println("║    TESTOK           - Test click tone              ║");
    Serial.println("║    TESTERR          - Test error tone              ║");
    Serial.println("║    TESTALARM        - Test alarm tone              ║");
    Serial.println("╚════════════════════════════════════════════════════╝\n");
    return;
  }
  
  sendFeedbackf("[Command] Unknown: %s", cmd.c_str());
  toneError();
}

// ===== BLYNK =====
BLYNK_CONNECTED() {
  Blynk.syncVirtual(V0, V1, V2, V3, V4, V5, V6);
}

BLYNK_WRITE(V0) { setRelay(0, param.asInt()); }
BLYNK_WRITE(V1) { setRelay(1, param.asInt()); }
BLYNK_WRITE(V2) { setRelay(2, param.asInt()); }
BLYNK_WRITE(V3) { setRelay(3, param.asInt()); }

BLYNK_WRITE(V10) {
  String cmd = param.asString();
  cmd.trim();
  cmd.toUpperCase();
  
  Serial.println("[Blynk V10 CMD] " + cmd);
  processSerialCommand(cmd);
  // All feedback is automatically sent to V20 via sendFeedback()
}

// ===== BLYNK VIRTUAL PINS =====
// V10: Command handler (processes commands like V11)
// V11: Command input - use Styled Button or Input Box
//      Example: R1 ON, A1 12:30, T 15:45, S+, STATUS
// V20: Status display - use Terminal widget (read-only)
//      Shows all command feedback automatically via sendFeedback()

BLYNK_WRITE(V11) {
  // Command input from Blynk
  String cmd = param.asString();
  cmd.trim();
  cmd.toUpperCase();
  
  sendFeedbackf("[Blynk V11 CMD] %s", cmd.c_str());
  processSerialCommand(cmd);
}

// ===== SETUP =====
void setup() {
  Serial.begin(9600);
  delay(1000);
  Serial.println("\n╔════════════════════════════════════════════╗");
  Serial.println("║  ESP8266 Smart Relay Controller v7.0      ║");
  Serial.println("║  Initializing...                           ║");
  Serial.println("╚════════════════════════════════════════════╝\n");
  
  // GPIO Setup - RELAYS & BUZZER
  for (int i = 0; i < 4; i++) {
    pinMode(RELAY_PINS[i], OUTPUT);
    digitalWrite(RELAY_PINS[i], LOW);
  }
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);
  pinMode(WIFI_LED, OUTPUT);
  digitalWrite(WIFI_LED, LOW);
  
  // GPIO Setup - BUTTONS
  for (int i = 0; i < 5; i++) {
    pinMode(BUTTON_PINS[i], INPUT_PULLUP);
    Serial.printf("Button %d (GPIO-%d): %s - Configured\n", i + 1, BUTTON_GPIO[i], BUTTON_NAMES[i]);
  }
  
  // Startup beep
  Serial.println("\nStartup: Beeping...");
  toneSystemStartup();
  delay(200);
  
  // WiFi Setup
  Serial.println("WiFi: Connecting...");
  toneSystemStartup();
  delay(200);
  
  WiFi.mode(WIFI_STA);
  delay(100);
  
  // Try reconnecting to saved credentials first
  unsigned long start = millis();
  WiFi.begin();
  while (!WiFi.isConnected() && millis() - start < 15000) {
    delay(100);
    Serial.print(".");
    yield();
  }
  Serial.println();
  
  if (!WiFi.isConnected()) {
    Serial.println("WiFi: Launching WiFiManager...");
    WiFiManager wm;
    wm.setConfigPortalTimeout(60);  // Reduced from 180 to 60 seconds
    wm.setSaveConfigCallback([]() {
      Serial.println("✓ WiFi Credentials Saved!");
    });
    wm.setConnectTimeout(20);
    wm.autoConnect("ESP_SmartRelay", "12345678");
  }
  
  delay(500);
  if (WiFi.isConnected()) {
    wifiConnected = true;
    wifiName = WiFi.SSID();
    Serial.printf("\n✓ WiFi Connected: %s\n", wifiName.c_str());
    Serial.printf("  IP: %s\n", WiFi.localIP().toString().c_str());
    digitalWrite(WIFI_LED, HIGH);
    
    // Blynk Setup
    Blynk.config(auth);
    Blynk.connect();
    
    // Connected: 3 short beeps
    Serial.println("✓ Blynk: Connecting...");
    toneInternetConnected();
  } else {
    Serial.println("\n✗ WiFi: Connection Failed - Running in Offline Mode");
    // Failed: SOS
    toneInternetDisconnect();
  }
  
  // Default Schedules
  relaySchedules[0].onHour = 21;
  relaySchedules[0].offHour = 0;
  relaySchedules[1].offHour = 2;
  
  // Default Alarms
  alarms[0].hour = 6;
  alarms[0].min = 0;
  alarms[0].active = true;
  
  // Timers
  timer.setInterval(1000L, updateTime);
  // Removed: auto dashboard print - only show when user requests
  timer.setInterval(5000L, [] {
    if (wifiConnected != WiFi.isConnected()) {
      wifiConnected = WiFi.isConnected();
      digitalWrite(WIFI_LED, wifiConnected ? HIGH : LOW);
      Serial.printf("[WiFi] Status: %s\n", wifiConnected ? "Connected" : "Disconnected");
    }
  });
  
  // Alarm & Schedule checks
  timer.setInterval(500L, checkAlarms);
  timer.setInterval(500L, checkSchedules);
  
  Serial.println("\n╔════════════════════════════════════════════╗");
  Serial.println("║  ✓ Setup Complete - Ready!                 ║");
  Serial.println("║  Type 'H' for commands                     ║");
  Serial.println("╚════════════════════════════════════════════╝\n");
  printDashboard();
}

// ===== LOOP =====
void loop() {
  beep_update();
  handleButtons();  // New: Button handling
  timer.run();
  
  if (wifiConnected && Blynk.connected()) {
    Blynk.run();
  }
  
  if (Serial.available()) {
    String cmd = Serial.readStringUntil('\n');
    processSerialCommand(cmd);
  }
  
  checkAlarms();      // New: Check alarms
  checkSchedules();   // New: Check schedules
  
  yield();
}
