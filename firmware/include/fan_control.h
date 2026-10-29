#ifndef FAN_CONTROL_H
#define FAN_CONTROL_H

#include <Arduino.h>
#include "sensors.h"

enum FanMode {
    FAN_MODE_AUTO = 0,
    FAN_MODE_MANUAL = 1
};

enum FanState {
    FAN_STATE_OFF = 0,
    FAN_STATE_ON = 1
};

struct FanStatus {
    FanMode mode;
    FanState state;
    const char* triggerReason;
};

void initFanControl();
void setFanState(FanState state, const char* reason = "Manual Override");
void setFanMode(FanMode mode);
FanStatus updateFanAutomatic(const SensorData& data);
FanStatus getFanStatus();

#endif // FAN_CONTROL_H

