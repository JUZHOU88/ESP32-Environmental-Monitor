#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include "MonitorCore.h"

// ESP32 DevKit example; adjust pin assignments for your board.
constexpr uint8_t kDoorPin = 27;   // dry contact to GND; INPUT_PULLUP
constexpr uint8_t kBuzzerLedPin = 2; // LED only by default. Do not directly drive a buzzer/relay.
constexpr uint8_t kSensorPin = 4; // DS18B20 DATA with 4.7k pull-up to 3.3V
constexpr uint32_t kSampleEveryMs = 2500;
constexpr uint32_t kPrintEveryMs = 1000;

OneWire oneWire(kSensorPin);
DallasTemperature thermometer(&oneWire);
monitor::MonitorCore controller;
uint32_t lastSample = 0;
uint32_t lastPrint = 0;
bool conversionPending = false;

void setup() {
    Serial.begin(115200);
    pinMode(kDoorPin, INPUT_PULLUP);
    pinMode(kBuzzerLedPin, OUTPUT);
    digitalWrite(kBuzzerLedPin, LOW);
    thermometer.begin();
    thermometer.setResolution(10); // <= 188ms typical conversion
    thermometer.setWaitForConversion(false);
    thermometer.requestTemperatures();
    conversionPending = true;
    lastSample = millis();
    Serial.println("ESP32 Environmental Monitor — demo firmware");
}

void loop() {
    const uint32_t now = millis();
    controller.setDoorSignal(digitalRead(kDoorPin) == LOW, now);

    if (conversionPending && static_cast<uint32_t>(now - lastSample) >= 250) {
        const float c = thermometer.getTempCByIndex(0);
        controller.setTemperature(c, c != DEVICE_DISCONNECTED_C && c > -55.0f && c < 125.0f, now);
        conversionPending = false;
    }
    if (!conversionPending && static_cast<uint32_t>(now - lastSample) >= kSampleEveryMs) {
        thermometer.requestTemperatures();
        conversionPending = true;
        lastSample = now;
    }

    auto state = controller.evaluate(now);
    // Flash a built-in/attached low-current LED to indicate the alarm state.
    // Do not attach a relay, motor or mains equipment directly to this GPIO.
    digitalWrite(kBuzzerLedPin, state.alarm && ((now / 250) % 2 == 0) ? HIGH : LOW);
    if (static_cast<uint32_t>(now - lastPrint) >= kPrintEveryMs) {
        lastPrint = now;
        Serial.printf("temp=%.2f C, sensorFault=%d, doorOpen=%d, tempAlarm=%d, doorAlarm=%d\n",
            state.temperatureC, state.sensorFault, state.doorOpen,
            state.temperatureAlarm, state.doorAlarm);
    }
}
