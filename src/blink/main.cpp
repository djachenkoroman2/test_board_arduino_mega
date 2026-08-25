#include <Arduino.h>

namespace {
constexpr unsigned long kBlinkIntervalMs = 500;
}

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  digitalWrite(LED_BUILTIN, HIGH);
  delay(kBlinkIntervalMs);
  digitalWrite(LED_BUILTIN, LOW);
  delay(kBlinkIntervalMs);
}
