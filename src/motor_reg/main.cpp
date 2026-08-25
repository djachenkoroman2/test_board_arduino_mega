#include <Arduino.h>
#include <Servo.h>

namespace {

constexpr uint8_t kEscSignalPin = 2;
constexpr unsigned long kSerialBaudRate = 115200UL;

constexpr int kMinimumPulseUs = 1000;
constexpr int kMaximumPulseUs = 2000;
constexpr int kPulseStepUs = 10;

constexpr unsigned long kRampStepIntervalMs = 50UL;
constexpr unsigned long kArmingDurationMs = 5000UL;
constexpr unsigned long kMaximumHoldDurationMs = 2000UL;
constexpr unsigned long kMinimumHoldDurationMs = 3000UL;
constexpr unsigned long kTelemetryIntervalMs = 500UL;

static_assert(kMinimumPulseUs < kMaximumPulseUs,
              "Minimum ESC pulse must be less than maximum");
static_assert(kPulseStepUs > 0, "ESC pulse step must be positive");

enum class State : uint8_t {
  Arming,
  RampUp,
  HoldMaximum,
  RampDown,
  HoldMinimum,
};

Servo esc;
State currentState = State::Arming;
int currentPulseUs = kMinimumPulseUs;
unsigned long stateStartedAtMs = 0;
unsigned long lastRampStepAtMs = 0;
unsigned long lastTelemetryAtMs = 0;

const __FlashStringHelper* stateName(State state) {
  switch (state) {
    case State::Arming:
      return F("arming");
    case State::RampUp:
      return F("ramp up");
    case State::HoldMaximum:
      return F("hold maximum");
    case State::RampDown:
      return F("ramp down");
    case State::HoldMinimum:
      return F("hold minimum");
  }

  return F("unknown");
}

int clampPulse(int requestedPulseUs) {
  if (requestedPulseUs < kMinimumPulseUs) {
    return kMinimumPulseUs;
  }
  if (requestedPulseUs > kMaximumPulseUs) {
    return kMaximumPulseUs;
  }
  return requestedPulseUs;
}

void setEscPulse(int requestedPulseUs) {
  currentPulseUs = clampPulse(requestedPulseUs);
  esc.writeMicroseconds(currentPulseUs);
}

void printStatus(const __FlashStringHelper* prefix) {
  Serial.print(prefix);
  Serial.print(F(": state="));
  Serial.print(stateName(currentState));
  Serial.print(F(", pulse="));
  Serial.print(currentPulseUs);
  Serial.println(F(" us"));
}

void enterState(State nextState, unsigned long nowMs) {
  currentState = nextState;
  stateStartedAtMs = nowMs;
  lastRampStepAtMs = nowMs;
  printStatus(F("Transition"));
}

bool elapsed(unsigned long nowMs, unsigned long startedAtMs,
             unsigned long durationMs) {
  return nowMs - startedAtMs >= durationMs;
}

void updateStateMachine(unsigned long nowMs) {
  switch (currentState) {
    case State::Arming:
      if (elapsed(nowMs, stateStartedAtMs, kArmingDurationMs)) {
        enterState(State::RampUp, nowMs);
      }
      break;

    case State::RampUp:
      if (elapsed(nowMs, lastRampStepAtMs, kRampStepIntervalMs)) {
        lastRampStepAtMs = nowMs;
        setEscPulse(currentPulseUs + kPulseStepUs);
        if (currentPulseUs >= kMaximumPulseUs) {
          enterState(State::HoldMaximum, nowMs);
        }
      }
      break;

    case State::HoldMaximum:
      if (elapsed(nowMs, stateStartedAtMs, kMaximumHoldDurationMs)) {
        enterState(State::RampDown, nowMs);
      }
      break;

    case State::RampDown:
      if (elapsed(nowMs, lastRampStepAtMs, kRampStepIntervalMs)) {
        lastRampStepAtMs = nowMs;
        setEscPulse(currentPulseUs - kPulseStepUs);
        if (currentPulseUs <= kMinimumPulseUs) {
          enterState(State::HoldMinimum, nowMs);
        }
      }
      break;

    case State::HoldMinimum:
      if (elapsed(nowMs, stateStartedAtMs, kMinimumHoldDurationMs)) {
        enterState(State::RampUp, nowMs);
      }
      break;
  }
}

void reportTelemetry(unsigned long nowMs) {
  if (!elapsed(nowMs, lastTelemetryAtMs, kTelemetryIntervalMs)) {
    return;
  }

  lastTelemetryAtMs = nowMs;
  printStatus(F("Status"));
}

}  // namespace

void setup() {
  Serial.begin(kSerialBaudRate);

  // Preload the safe pulse before enabling output on the signal pin.
  esc.writeMicroseconds(kMinimumPulseUs);
  esc.attach(kEscSignalPin, kMinimumPulseUs, kMaximumPulseUs);
  setEscPulse(kMinimumPulseUs);

  const unsigned long nowMs = millis();
  stateStartedAtMs = nowMs;
  lastRampStepAtMs = nowMs;
  lastTelemetryAtMs = nowMs;

  Serial.println(F("ESC motor regulator test"));
  printStatus(F("Initial"));
}

void loop() {
  const unsigned long nowMs = millis();
  updateStateMachine(nowMs);
  reportTelemetry(nowMs);
}
