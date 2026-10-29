#include <Arduino.h>
#include "config.h"
#include "sensors.h"
#include "fan_control.h"
#include "network.h"

static unsigned long lastSensorRead = 0;
static unsigned long lastTelemetryPublish = 0;

static SensorData currentSensorData;
static FanStatus currentFanStatus;

void setup() {
    Serial.begin(115200);
    delay(1000);
    Serial.println("\n==========================================");
    Serial.println("  AirVent: Sistem Exhaust Fan Otomatis   ");
    Serial.println("  Monitoring Suhu & Kualitas Udara ESP32  ");
    Serial.println("==========================================\n");

    pinMode(PIN_LED_STATUS, OUTPUT);
    digitalWrite(PIN_LED_STATUS, LOW);

    // Inisialisasi modul
    initFanControl();
    initSensors();
    initNetwork();

    digitalWrite(PIN_LED_STATUS, HIGH); // Indikator setup selesai
}

void loop() {
    unsigned long currentMillis = millis();

    // 1. Maintain koneksi WiFi & MQTT
    handleNetworkLoop();

    // 2. Baca Sensor secara periodik (non-blocking)
    if (currentMillis - lastSensorRead >= SENSOR_READ_INTERVAL_MS) {
        lastSensorRead = currentMillis;

        currentSensorData = readSensors();
        
        // Tampilkan log ke Serial Monitor
        if (currentSensorData.isValid) {
            Serial.printf("[DATA] Suhu: %.1f°C | Kelembaban: %.1f%% | Gas: %.1f PPM | Status: %s\n",
                          currentSensorData.temperature,
                          currentSensorData.humidity,
                          currentSensorData.gasPpm,
                          currentSensorData.airQualityText);
        }

        // Perbarui kondisi otomatis exhaust fan
        currentFanStatus = updateFanAutomatic(currentSensorData);
    }

    // 3. Kirim Telemetry ke Cloud/MQTT Broker
    if (currentMillis - lastTelemetryPublish >= TELEMETRY_PUBLISH_MS) {
        lastTelemetryPublish = currentMillis;

        if (currentSensorData.isValid) {
            publishTelemetry(currentSensorData, currentFanStatus);
        }
    }
}

