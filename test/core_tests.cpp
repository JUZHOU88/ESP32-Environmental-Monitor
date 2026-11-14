#include "MonitorCore.h"
#include <cassert>
#include <cmath>
#include <iostream>
#include <limits>
#include <stdexcept>
using namespace monitor;

int main() {
    {
        MonitorCore m;
        assert(m.evaluate(0).sensorFault);
        m.setTemperature(22, true, 0);
        assert(!m.evaluate(1).alarm);
        assert(m.evaluate(15001).sensorFault);
        m.setTemperature(22, false, 15002);
        assert(m.evaluate(15002).sensorFault);
        m.setTemperature(std::numeric_limits<float>::quiet_NaN(), true, 15003);
        assert(m.evaluate(15003).sensorFault);
    }
    {
        MonitorCore m;
        m.setTemperature(29, true, 0);
        assert(m.evaluate(0).temperatureAlarm);
        m.setTemperature(27.8, true, 10);
        assert(m.evaluate(10).temperatureAlarm); // hysteresis prevents chatter
        m.setTemperature(27.4, true, 20);
        assert(!m.evaluate(20).temperatureAlarm);
        m.setTemperature(17, true, 30);
        assert(m.evaluate(30).temperatureAlarm);
        m.setTemperature(18.6, true, 40);
        assert(!m.evaluate(40).temperatureAlarm);
    }
    {
        MonitorCore m;
        m.setTemperature(22, true, 0);
        m.setDoorSignal(true, 0);
        m.setDoorSignal(false, 40); // switch bounce
        assert(!m.evaluate(41).doorOpen);
        m.setDoorSignal(true, 100);
        m.setDoorSignal(true, 161); // debounced
        assert(m.evaluate(161).doorOpen);
        assert(!m.evaluate(5159).doorAlarm);
        assert(m.evaluate(5161).doorAlarm);
        m.setDoorSignal(false, 5200);
        m.setDoorSignal(false, 5270);
        assert(!m.evaluate(5270).doorAlarm);
    }
    {
        // unsigned millisecond subtraction handles 32-bit timer rollover
        MonitorCore m;
        uint32_t nearWrap = UINT32_MAX - 20;
        m.setTemperature(22, true, nearWrap);
        assert(!m.evaluate(30).sensorFault);
    }
    {
        bool rejected = false;
        try { MonitorCore invalid({30, 20, .5f, 15000, 5000, 60}); }
        catch (const std::invalid_argument&) { rejected = true; }
        assert(rejected);
    }
    std::cout << "All 5 MonitorCore test groups passed.\n";
}
