#ifndef SENSORS_H
#define SENSORS_H

#include <Arduino.h>

enum AirQualityCategory {
    AIR_GOOD = 0,      // Udara Bersih / Baik
    AIR_MODERATE = 1,  // Udara Sedang
    AIR_POOR = 2       // Udara Buruk / Perlu Ventilasi
};

struct SensorData {
    float temperature;
    float humidity;
    int rawGasValue;
    float gasPpm;
    AirQualityCategory airQuality;
    const char* airQualityText;
    bool isValid;
};

void initSensors();
SensorData readSensors();
AirQualityCategory evaluateAirQuality(float ppm);
const char* airQualityToString(AirQualityCategory category);

#endif // SENSORS_H

