#include <Arduino.h>

namespace {
constexpr unsigned long kReportIntervalMs = 1000;
}

void setup() {
  Serial.begin(115200);
}

void loop() {
  Serial.println(F("Arduino Mega is running"));
  delay(kReportIntervalMs);
}
