/*
 * Smart Relay Controller - Unit Tests
 * Test các hàm logic mà không cần hardware
 * 
 * Compile & Run:
 *   platformio test -e nodemcuv2
 */

#include <Arduino.h>
#include <unity.h>

// ===== MOCK DATA =====
bool relayState[4] = {false, false, false, false};
int currentHour = 12, currentMin = 0, currentSec = 0;
bool timerControlEnabled = true;
bool silentMode = false;

struct {
  int onHour = -1, onMin = 0;
  int offHour = -1, offMin = 0;
} relaySchedules[4];

struct {
  int hour = -1, min = 0;
  bool active = true;
} alarms[2];

const char* relayNames[4] = {"LIGHT", "FAN", "PUMP", "TV"};

// ===== FUNCTION SIGNATURES =====
// Khai báo các hàm sẽ test
void setRelayState(int index, bool state) {
  if (index >= 0 && index < 4) {
    relayState[index] = state;
  }
}

bool isScheduleTriggered(int relayIdx) {
  if (relayIdx < 0 || relayIdx >= 4) return false;
  if (!timerControlEnabled) return false;
  if (relaySchedules[relayIdx].onHour < 0) return false;
  
  return (currentHour == relaySchedules[relayIdx].onHour && 
          currentMin == relaySchedules[relayIdx].onMin && 
          currentSec == 0);
}

bool isAlarmTriggered(int alarmIdx) {
  if (alarmIdx < 0 || alarmIdx >= 2) return false;
  if (!alarms[alarmIdx].active) return false;
  if (alarms[alarmIdx].hour < 0) return false;
  
  return (currentHour == alarms[alarmIdx].hour && 
          currentMin == alarms[alarmIdx].min && 
          currentSec == 0);
}

bool parseTimeCommand(String cmd, int &hour, int &minute) {
  // Parse format: "15:45"
  int colonPos = cmd.indexOf(':');
  if (colonPos <= 0) return false;
  
  hour = cmd.substring(0, colonPos).toInt();
  minute = cmd.substring(colonPos + 1).toInt();
  
  return (hour >= 0 && hour < 24 && minute >= 0 && minute < 60);
}

bool parseScheduleCommand(String cmd, int &relayIdx, int &onH, int &onM, int &offH, int &offM) {
  // Parse format: "SC1 ON 21:00 OFF 06:00"
  if (cmd.length() < 3 || cmd[0] != 'S' || cmd[1] != 'C') return false;
  
  relayIdx = cmd[2] - '0' - 1;
  if (relayIdx < 0 || relayIdx >= 4) return false;
  
  int onPos = cmd.indexOf("ON");
  int offPos = cmd.indexOf("OFF");
  
  if (onPos < 0 || offPos < 0 || offPos <= onPos) return false;
  
  String onTimeStr = cmd.substring(onPos + 2, offPos);
  String offTimeStr = cmd.substring(offPos + 3);
  
  onTimeStr.trim();
  offTimeStr.trim();
  
  int colonPos1 = onTimeStr.indexOf(':');
  int colonPos2 = offTimeStr.indexOf(':');
  
  if (colonPos1 <= 0 || colonPos2 <= 0) return false;
  
  onH = onTimeStr.substring(0, colonPos1).toInt();
  onM = onTimeStr.substring(colonPos1 + 1).toInt();
  offH = offTimeStr.substring(0, colonPos2).toInt();
  offM = offTimeStr.substring(colonPos2 + 1).toInt();
  
  return (onH >= 0 && onH < 24 && onM >= 0 && onM < 60 &&
          offH >= 0 && offH < 24 && offM >= 0 && offM < 60);
}

// ===== TEST SETUP & TEARDOWN =====
void setUp(void) {
  // Reset trước mỗi test
  for (int i = 0; i < 4; i++) {
    relayState[i] = false;
    relaySchedules[i].onHour = -1;
    relaySchedules[i].offHour = -1;
  }
  for (int i = 0; i < 2; i++) {
    alarms[i].hour = -1;
    alarms[i].active = true;
  }
  currentHour = 12;
  currentMin = 0;
  currentSec = 0;
  timerControlEnabled = true;
  silentMode = false;
}

void tearDown(void) {
  // Cleanup (nếu cần)
}

// ===== UNIT TESTS =====

// TEST 1: Relay Control
void test_relay_on(void) {
  setRelayState(0, true);
  TEST_ASSERT_TRUE(relayState[0]);
  TEST_ASSERT_FALSE(relayState[1]);
}

void test_relay_off(void) {
  setRelayState(0, true);
  setRelayState(0, false);
  TEST_ASSERT_FALSE(relayState[0]);
}

void test_relay_invalid_index(void) {
  setRelayState(-1, true);
  setRelayState(4, true);
  TEST_ASSERT_FALSE(relayState[0]);
  TEST_ASSERT_FALSE(relayState[3]);
}

void test_relay_toggle_all(void) {
  for (int i = 0; i < 4; i++) {
    setRelayState(i, true);
  }
  TEST_ASSERT_TRUE(relayState[0]);
  TEST_ASSERT_TRUE(relayState[1]);
  TEST_ASSERT_TRUE(relayState[2]);
  TEST_ASSERT_TRUE(relayState[3]);
}

// TEST 2: Schedule Triggering
void test_schedule_trigger_on_time(void) {
  relaySchedules[0].onHour = 12;
  relaySchedules[0].onMin = 0;
  currentHour = 12;
  currentMin = 0;
  currentSec = 0;
  
  TEST_ASSERT_TRUE(isScheduleTriggered(0));
}

void test_schedule_not_trigger_before_time(void) {
  relaySchedules[0].onHour = 12;
  relaySchedules[0].onMin = 0;
  currentHour = 11;
  currentMin = 59;
  currentSec = 59;
  
  TEST_ASSERT_FALSE(isScheduleTriggered(0));
}

void test_schedule_not_trigger_after_time(void) {
  relaySchedules[0].onHour = 12;
  relaySchedules[0].onMin = 0;
  currentHour = 12;
  currentMin = 0;
  currentSec = 1;
  
  TEST_ASSERT_FALSE(isScheduleTriggered(0));
}

void test_schedule_disabled_when_timer_off(void) {
  relaySchedules[0].onHour = 12;
  relaySchedules[0].onMin = 0;
  currentHour = 12;
  currentMin = 0;
  currentSec = 0;
  timerControlEnabled = false;
  
  TEST_ASSERT_FALSE(isScheduleTriggered(0));
}

void test_schedule_not_trigger_if_not_set(void) {
  relaySchedules[0].onHour = -1;
  currentHour = 12;
  currentMin = 0;
  currentSec = 0;
  
  TEST_ASSERT_FALSE(isScheduleTriggered(0));
}

void test_schedule_with_minutes(void) {
  relaySchedules[0].onHour = 14;
  relaySchedules[0].onMin = 30;
  currentHour = 14;
  currentMin = 30;
  currentSec = 0;
  
  TEST_ASSERT_TRUE(isScheduleTriggered(0));
}

void test_schedule_minutes_not_match(void) {
  relaySchedules[0].onHour = 14;
  relaySchedules[0].onMin = 30;
  currentHour = 14;
  currentMin = 29;
  currentSec = 0;
  
  TEST_ASSERT_FALSE(isScheduleTriggered(0));
}

// TEST 3: Alarm Triggering
void test_alarm_trigger_on_time(void) {
  alarms[0].hour = 6;
  alarms[0].min = 30;
  alarms[0].active = true;
  currentHour = 6;
  currentMin = 30;
  currentSec = 0;
  
  TEST_ASSERT_TRUE(isAlarmTriggered(0));
}

void test_alarm_not_trigger_if_inactive(void) {
  alarms[0].hour = 6;
  alarms[0].min = 30;
  alarms[0].active = false;
  currentHour = 6;
  currentMin = 30;
  currentSec = 0;
  
  TEST_ASSERT_FALSE(isAlarmTriggered(0));
}

void test_alarm_not_trigger_if_not_set(void) {
  alarms[0].hour = -1;
  alarms[0].active = true;
  currentHour = 6;
  currentMin = 30;
  currentSec = 0;
  
  TEST_ASSERT_FALSE(isAlarmTriggered(0));
}

void test_alarm_two_alarms(void) {
  alarms[0].hour = 6;
  alarms[0].min = 0;
  alarms[0].active = true;
  
  alarms[1].hour = 12;
  alarms[1].min = 30;
  alarms[1].active = true;
  
  currentHour = 6;
  currentMin = 0;
  currentSec = 0;
  
  TEST_ASSERT_TRUE(isAlarmTriggered(0));
  TEST_ASSERT_FALSE(isAlarmTriggered(1));
}

// TEST 4: Time Parsing
void test_parse_time_valid(void) {
  int h, m;
  String cmd = "15:45";
  bool result = parseTimeCommand(cmd, h, m);
  
  TEST_ASSERT_TRUE(result);
  TEST_ASSERT_EQUAL(15, h);
  TEST_ASSERT_EQUAL(45, m);
}

void test_parse_time_midnight(void) {
  int h, m;
  String cmd = "00:00";
  bool result = parseTimeCommand(cmd, h, m);
  
  TEST_ASSERT_TRUE(result);
  TEST_ASSERT_EQUAL(0, h);
  TEST_ASSERT_EQUAL(0, m);
}

void test_parse_time_invalid_hour(void) {
  int h, m;
  String cmd = "25:00";
  bool result = parseTimeCommand(cmd, h, m);
  
  TEST_ASSERT_FALSE(result);
}

void test_parse_time_invalid_minute(void) {
  int h, m;
  String cmd = "12:60";
  bool result = parseTimeCommand(cmd, h, m);
  
  TEST_ASSERT_FALSE(result);
}

void test_parse_time_no_colon(void) {
  int h, m;
  String cmd = "1245";
  bool result = parseTimeCommand(cmd, h, m);
  
  TEST_ASSERT_FALSE(result);
}

// TEST 5: Schedule Parsing
void test_parse_schedule_valid(void) {
  int idx, onH, onM, offH, offM;
  String cmd = "SC1 ON 21:00 OFF 06:00";
  bool result = parseScheduleCommand(cmd, idx, onH, onM, offH, offM);
  
  TEST_ASSERT_TRUE(result);
  TEST_ASSERT_EQUAL(0, idx);      // Relay 1 = index 0
  TEST_ASSERT_EQUAL(21, onH);
  TEST_ASSERT_EQUAL(0, onM);
  TEST_ASSERT_EQUAL(6, offH);
  TEST_ASSERT_EQUAL(0, offM);
}

void test_parse_schedule_relay_2(void) {
  int idx, onH, onM, offH, offM;
  String cmd = "SC2 ON 19:30 OFF 23:45";
  bool result = parseScheduleCommand(cmd, idx, onH, onM, offH, offM);
  
  TEST_ASSERT_TRUE(result);
  TEST_ASSERT_EQUAL(1, idx);      // Relay 2 = index 1
  TEST_ASSERT_EQUAL(19, onH);
  TEST_ASSERT_EQUAL(30, onM);
  TEST_ASSERT_EQUAL(23, offH);
  TEST_ASSERT_EQUAL(45, offM);
}

void test_parse_schedule_invalid_relay(void) {
  int idx, onH, onM, offH, offM;
  String cmd = "SC5 ON 21:00 OFF 06:00";
  bool result = parseScheduleCommand(cmd, idx, onH, onM, offH, offM);
  
  TEST_ASSERT_FALSE(result);
}

void test_parse_schedule_missing_on(void) {
  int idx, onH, onM, offH, offM;
  String cmd = "SC1 21:00 OFF 06:00";
  bool result = parseScheduleCommand(cmd, idx, onH, onM, offH, offM);
  
  TEST_ASSERT_FALSE(result);
}

void test_parse_schedule_missing_off(void) {
  int idx, onH, onM, offH, offM;
  String cmd = "SC1 ON 21:00 06:00";
  bool result = parseScheduleCommand(cmd, idx, onH, onM, offH, offM);
  
  TEST_ASSERT_FALSE(result);
}

void test_parse_schedule_invalid_time(void) {
  int idx, onH, onM, offH, offM;
  String cmd = "SC1 ON 25:00 OFF 06:00";
  bool result = parseScheduleCommand(cmd, idx, onH, onM, offH, offM);
  
  TEST_ASSERT_FALSE(result);
}

// ===== TEST MAIN =====
void setup() {
  delay(2000);
  UNITY_BEGIN();
  
  // Relay Tests
  RUN_TEST(test_relay_on);
  RUN_TEST(test_relay_off);
  RUN_TEST(test_relay_invalid_index);
  RUN_TEST(test_relay_toggle_all);
  
  // Schedule Tests
  RUN_TEST(test_schedule_trigger_on_time);
  RUN_TEST(test_schedule_not_trigger_before_time);
  RUN_TEST(test_schedule_not_trigger_after_time);
  RUN_TEST(test_schedule_disabled_when_timer_off);
  RUN_TEST(test_schedule_not_trigger_if_not_set);
  RUN_TEST(test_schedule_with_minutes);
  RUN_TEST(test_schedule_minutes_not_match);
  
  // Alarm Tests
  RUN_TEST(test_alarm_trigger_on_time);
  RUN_TEST(test_alarm_not_trigger_if_inactive);
  RUN_TEST(test_alarm_not_trigger_if_not_set);
  RUN_TEST(test_alarm_two_alarms);
  
  // Time Parsing Tests
  RUN_TEST(test_parse_time_valid);
  RUN_TEST(test_parse_time_midnight);
  RUN_TEST(test_parse_time_invalid_hour);
  RUN_TEST(test_parse_time_invalid_minute);
  RUN_TEST(test_parse_time_no_colon);
  
  // Schedule Parsing Tests
  RUN_TEST(test_parse_schedule_valid);
  RUN_TEST(test_parse_schedule_relay_2);
  RUN_TEST(test_parse_schedule_invalid_relay);
  RUN_TEST(test_parse_schedule_missing_on);
  RUN_TEST(test_parse_schedule_missing_off);
  RUN_TEST(test_parse_schedule_invalid_time);
  
  UNITY_END();
}

void loop() {
  // Nothing to do
}
