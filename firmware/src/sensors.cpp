#include "sensors.h"
#include "config.h"
#include <DHT.h>

static DHT dht(PIN_DHT, DHT_TYPE);

void initSensors() {
    Serial.println("[SENSOR] Menginisialisasi sensor DHT dan MQ-135...");
    dht.begin();
    pinMode(PIN_MQ135, INPUT);
    // Beri jeda sejenak untuk stabilisasi sensor
    delay(1000);
    Serial.println("[SENSOR] Sensor siap digunakan.");
}

AirQualityCategory evaluateAirQuality(float ppm) {
    if (ppm < 200.0f) {
        return AIR_GOOD;
    } else if (ppm <= AQI_THRESHOLD_POOR) {
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

    // Membaca suhu dan kelembaban dari sensor DHT
    float temp = dht.readTemperature();
    float hum = dht.readHumidity();

    if (isnan(temp) || isnan(hum)) {
        Serial.println("[SENSOR ERROR] Gagal membaca data dari sensor DHT!");
        data.temperature = 0.0f;
        data.humidity = 0.0f;
    } else {
        data.temperature = temp;
        data.humidity = hum;
        data.isValid = true;
    }

    // Membaca nilai analog dari sensor MQ-135 (Rentang ADC ESP32: 0 - 4095)
    int rawGas = analogRead(PIN_MQ135);
    data.rawGasValue = rawGas;

    // Perhitungan estimasi PPM gas (skala proporsional sederhana untuk demonstrasi)
    // Untuk pengukuran presisi, lakukan kalibrasi resistansi sensor (Rs/Ro) di docs/calibration_guide.md
    float estimatedPpm = map(rawGas, 0, 4095, 50, 1000);
    data.gasPpm = estimatedPpm;

    data.airQuality = evaluateAirQuality(estimatedPpm);
    data.airQualityText = airQualityToString(data.airQuality);

    return data;
}

