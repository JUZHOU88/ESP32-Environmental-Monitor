#include "MonitorCore.h"
#include <cmath>
#include <stdexcept>

namespace monitor {

MonitorCore::MonitorCore(Config config) : config_(config) {
    if (!(std::isfinite(config.minimumC) && std::isfinite(config.maximumC) &&
          std::isfinite(config.hysteresisC)) || config.minimumC >= config.maximumC ||
        config.hysteresisC < 0 ||
        config.hysteresisC * 2 >= config.maximumC - config.minimumC)
        throw std::invalid_argument("Invalid temperature configuration");
}

void MonitorCore::setTemperature(float celsius, bool valid, uint32_t nowMs) {
    sampleSeen_ = true;
    sampleValid_ = valid && std::isfinite(celsius);
    sampleAtMs_ = nowMs;
    if (sampleValid_) lastTemperatureC_ = celsius;
}

void MonitorCore::setDoorSignal(bool open, uint32_t nowMs) {
    if (open != candidateOpen_) {
        candidateOpen_ = open;
        candidateSinceMs_ = nowMs;
    }
    if (stableOpen_ != candidateOpen_ &&
        static_cast<uint32_t>(nowMs - candidateSinceMs_) >= config_.debounceMs) {
        stableOpen_ = candidateOpen_;
        if (stableOpen_) openSinceMs_ = nowMs;
    }
}

Snapshot MonitorCore::evaluate(uint32_t nowMs) {
    const bool fault = !sampleSeen_ || !sampleValid_ ||
        static_cast<uint32_t>(nowMs - sampleAtMs_) > config_.sampleStaleMs;
    if (fault) {
        temperatureAlarm_ = false;
    } else if (temperatureAlarm_) {
        // Clear only after moving well inside the normal range.
        temperatureAlarm_ = lastTemperatureC_ < config_.minimumC + config_.hysteresisC ||
            lastTemperatureC_ > config_.maximumC - config_.hysteresisC;
    } else {
        temperatureAlarm_ = lastTemperatureC_ < config_.minimumC ||
            lastTemperatureC_ > config_.maximumC;
    }
    const bool doorAlarm = stableOpen_ &&
        static_cast<uint32_t>(nowMs - openSinceMs_) >= config_.doorDelayMs;
    return {fault, temperatureAlarm_, doorAlarm, stableOpen_,
            fault || temperatureAlarm_ || doorAlarm, lastTemperatureC_};
}

} // namespace monitor
