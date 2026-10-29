#ifndef NETWORK_H
#define NETWORK_H

#include <Arduino.h>
#include "sensors.h"
#include "fan_control.h"

void initNetwork();
void handleNetworkLoop();
bool isNetworkConnected();
void publishTelemetry(const SensorData& sensorData, const FanStatus& fanStatus);
void publishFanStatus(const FanStatus& fanStatus);

#endif // NETWORK_H

