#pragma once
#include <cstdint>

namespace monitor {

struct Config {
    float minimumC = 18.0f;       // illustrative indoor range, not medical guidance
    float maximumC = 28.0f;
    float hysteresisC = 0.5f;
    uint32_t sampleStaleMs = 15000;
    uint32_t doorDelayMs = 5000;
    uint32_t debounceMs = 60;
};

struct Snapshot {
    bool sensorFault;
    bool temperatureAlarm;
    bool doorAlarm;
    bool doorOpen;
    bool alarm;
    float temperatureC;
};

class MonitorCore {
public:
    explicit MonitorCore(Config config = {});
    void setTemperature(float celsius, bool valid, uint32_t nowMs);
    void setDoorSignal(bool open, uint32_t nowMs);
    Snapshot evaluate(uint32_t nowMs);
private:
    Config config_;
    bool sampleSeen_ = false;
    bool sampleValid_ = false;
    uint32_t sampleAtMs_ = 0;
    float lastTemperatureC_ = 0;
    bool temperatureAlarm_ = false;
    bool candidateOpen_ = false;
    bool stableOpen_ = false;
    uint32_t candidateSinceMs_ = 0;
    uint32_t openSinceMs_ = 0;
};

} // namespace monitor
