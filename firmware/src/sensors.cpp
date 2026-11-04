#include "sensors.h"
#include "config.h"
#include <OneWire.h>
#include <DallasTemperature.h>

static OneWire oneWire(PIN_DS18B20);
static DallasTemperature dsSensors(&oneWire);

void initSensors() {
    Serial.println("[SENSOR] Menginisialisasi sensor DS18B20 & Gas MQ-135 (Wokwi)...");
    dsSensors.begin();
    pinMode(PIN_MQ135, INPUT);
    delay(500);
    Serial.println("[SENSOR] Sensor DS18B20 & MQ-135 siap digunakan.");
}

AirQualityCategory evaluateAirQuality(float ppm) {
    if (ppm < 220.0f) {
        return AIR_GOOD;
    } else if (ppm < AQI_THRESHOLD_POOR) {
        return AIR_MODERATE;
    } else {
        return AIR_POOR;
    }
}

const char* airQualityToString(AirQualityCategory category) {
    switch (category) {
        case AIR_GOOD:     return "Baik (Good)";
        case AIR_MODERATE: return "Sedang (Moderate)";
        case AIR_POOR:     return "Buruk (Poor)";
        default:           return "Tidak Diketahui";
    }
}

SensorData readSensors() {
    SensorData data;
    data.isValid = false;

    // Membaca suhu dari sensor DS18B20 (Wokwi)
    dsSensors.requestTemperatures();
    float temp = dsSensors.getTempCByIndex(0);

    if (temp == DEVICE_DISCONNECTED_C || temp < -55.0f || temp > 125.0f) {
        Serial.println("[SENSOR WARNING] Gagal membaca DS18B20, menggunakan nilai acuan 28.0°C");
        data.temperature = 28.0f;
    } else {
        data.temperature = temp;
        data.isValid = true;
    }

    // Baseline estimasi kelembaban ruangan (~60%) agar kartu & chart kelembaban di web dashboard tetap aktif
    data.humidity = 60.0f;

    // Membaca nilai analog dari sensor MQ-135 (Rentang ADC ESP32: 0 - 4095)
    int rawGas = analogRead(PIN_MQ135);
    data.rawGasValue = rawGas;

    // Mapping nilai ADC ke skala estimasi PPM (50 - 1000 PPM)
    float estimatedPpm = map(rawGas, 0, 4095, 50, 1000);
    data.gasPpm = estimatedPpm;

    data.airQuality = evaluateAirQuality(estimatedPpm);
    data.airQualityText = airQualityToString(data.airQuality);

    return data;
}
