#ifndef AIRVENT_NET_H
#define AIRVENT_NET_H

#include <Arduino.h>
#include "sensors.h"
#include "fan_control.h"

void initNetwork();
void handleNetworkLoop();
void reconnectMqtt();
bool isNetworkConnected();

void publishTelemetry(const SensorData& sensorData, const FanStatus& fanStatus);
void publishFanStatus(const FanStatus& fanStatus);

#endif // AIRVENT_NET_H

