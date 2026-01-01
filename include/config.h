#ifndef CONFIG_H
#define CONFIG_H

// ╔════════════════════════════════════════════════════════════════╗
// ║       SMART RELAY v7.0 CONFIGURATION FILE                     ║
// ║  All sensitive data (tokens, passwords) stored here ONLY       ║
// ║  DO NOT commit real tokens to public repositories              ║
// ╚════════════════════════════════════════════════════════════════╝

// ===== BLYNK CONFIGURATION (KEEP SECURE!) =====
#define BLYNK_TEMPLATE_ID "TMPLxxxxxx"
#define BLYNK_TEMPLATE_NAME "Smart Home Control"
#define BLYNK_AUTH_TOKEN "Xd_ft-JArbZzZAuh0_TCXSoPRJjzB5u6"

// ===== TIMEZONE & NTP =====
#define TZ_OFFSET (7 * 3600)        // UTC+7 for Vietnam (Ho Chi Minh)
#define NTP_SERVER_1 "pool.ntp.org"
#define NTP_SERVER_2 "time.nist.gov"

// ===== RELAY CONFIGURATION =====
#define RELAY_1_NAME "LIGHT"
#define RELAY_2_NAME "FAN"
#define RELAY_3_NAME "PUMP"
#define RELAY_4_NAME "TV"

// ===== RELAY DEFAULT SCHEDULES =====
// Relay 1: ON at 21:00, OFF at 00:00
#define DEFAULT_R1_ON_HOUR 21
#define DEFAULT_R1_ON_MIN 0
#define DEFAULT_R1_OFF_HOUR 0
#define DEFAULT_R1_OFF_MIN 0

// Relay 2: OFF at 02:00
#define DEFAULT_R2_OFF_HOUR 2
#define DEFAULT_R2_OFF_MIN 0

// ===== ALARM DEFAULT =====
#define DEFAULT_ALARM_1_HOUR 6
#define DEFAULT_ALARM_1_MIN 0
#define DEFAULT_ALARM_1_ACTIVE true
#define DEFAULT_ALARM_2_HOUR -1
#define DEFAULT_ALARM_2_MIN 0
#define DEFAULT_ALARM_2_ACTIVE false

// ===== BUTTON CONFIGURATION =====
#define BUTTON_DEBOUNCE_MS 50
#define RESET_HOLD_TIME_MS 5000

// ===== SERIAL CONFIGURATION =====
#define SERIAL_BAUD 9600

// ===== FEATURE FLAGS =====
#define ENABLE_CHIMING true
#define ENABLE_ALARMS true
#define ENABLE_SCHEDULES true
#define ENABLE_NTP true
#define ENABLE_WIFI_MANAGER true

// ===== WIFI MANAGER =====
#define WIFIMANAGER_TIMEOUT_SEC 60
#define WIFI_CONNECT_TIMEOUT_SEC 20
#define WIFI_AP_NAME "ESP_SmartRelay"
#define WIFI_AP_PASSWORD "12345678"

// ===== BLYNK VIRTUAL PINS =====
// V0-V3:   Relay state switches (read/write)
// V4-V9:   Reserved for future use
// V10:     Command input & feedback
// V11:     Command input (alternative)
// V20:     Status/Terminal display (read-only)

// ===== DEBUG =====
#define DEBUG_MODE true

#if DEBUG_MODE
  #define DEBUG_PRINT(x) Serial.print(x)
  #define DEBUG_PRINTLN(x) Serial.println(x)
  #define DEBUG_PRINTF(...) Serial.printf(__VA_ARGS__)
#else
  #define DEBUG_PRINT(x)
  #define DEBUG_PRINTLN(x)
  #define DEBUG_PRINTF(...)
#endif

// ===== TONE SOUND DURATIONS (milliseconds) =====
// All tone sounds configurable - adjust these to customize beep patterns
#define TONE_SHORT_BEEP 100
#define TONE_LONG_BEEP 300
#define TONE_SHORT_DELAY 100
#define TONE_MEDIUM_DELAY 150
#define TONE_LONG_DELAY 200
#define TONE_EXTRA_LONG_DELAY 350
#define TONE_PAUSE_DELAY 300

// ===== TONE PATTERNS =====
// Error tone: 3 quick beeps
#define TONE_ERROR_COUNT 3
#define TONE_ERROR_BEEP_MS TONE_SHORT_BEEP
#define TONE_ERROR_INTERVAL_MS 50

// Mode Enter/Exit patterns
#define TONE_MODE_ENTER_BEEP_MS TONE_LONG_BEEP
#define TONE_MODE_EXIT_BEEPS 2
#define TONE_MODE_EXIT_FINAL_BEEP_MS TONE_LONG_BEEP

// Alarm patterns
#define TONE_ALARM_BEEP_COUNT 3
#define TONE_ALARM_PAUSE_MS 300
#define TONE_ALARM_REPEAT_COUNT 5
#define TONE_ALARM_CHIME_COUNT 2

// Internet/System patterns
#define TONE_INTERNET_SOS_LONG_MS TONE_LONG_BEEP
#define TONE_INTERNET_SOS_SHORT_MS TONE_SHORT_BEEP
#define TONE_SYSTEM_STARTUP_BEEP_MS TONE_LONG_BEEP

// Click/UX feedback
#define TONE_CLICK_BEEP_MS TONE_SHORT_BEEP
#define TONE_DOUBLE_CLICK_COUNT 2

#endif // CONFIG_H
