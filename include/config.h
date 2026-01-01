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

#endif // CONFIG_H
