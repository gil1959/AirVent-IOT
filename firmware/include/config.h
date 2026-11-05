#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

// ==================== KONFIGURASI WIFI ====================
// Wokwi WiFi simulation menggunakan "Wokwi-GUEST" tanpa password
#define WIFI_SSID           "Wokwi-GUEST"
#define WIFI_PASSWORD       ""

// ==================== KONFIGURASI MQTT BROKER ====================
// Selaras dengan Web Dashboard (EMQX Public Broker)
#define MQTT_BROKER         "broker.emqx.io"
#define MQTT_PORT           1883
#define MQTT_CLIENT_ID      "AirVent-ESP32-Wokwi"
#define MQTT_TOPIC_DATA     "airvent/telemetry"       // Topik publikasi telemetry sensor
#define MQTT_TOPIC_STATUS   "airvent/fan/status"      // Topik status kipas
#define MQTT_TOPIC_CONTROL  "airvent/fan/control"     // Topik kontrol manual dari Web Dashboard

// ==================== KONFIGURASI PINOUT GPIO ====================
// Sesuai dengan diagram.json Wokwi (Tanpa mengubah rangkaian)
#define PIN_DS18B20         4        // GPIO4 untuk Sensor Suhu DS18B20 (OneWire)
#define PIN_MQ135           34       // GPIO34 (ADC1) untuk Sensor Gas MQ-135
#define PIN_RELAY_FAN       26       // GPIO26 untuk Modul Relay Exhaust Fan
#define PIN_LED_STATUS      2        // GPIO2 untuk LED status ESP32

// ==================== PIN TAMBAHAN DEMO (BLOCKING vs NON-BLOCKING) ====================
#define PIN_BUTTON_TEST     19       // GPIO19 untuk Push Button Uji Respons Kipas
#define PIN_SWITCH_MODE     18       // GPIO18 untuk Switch Pemilih Mode (LOW=Blocking, HIGH=Non-Blocking)
#define PIN_LED_HEARTBEAT   23       // GPIO23 untuk LED Heartbeat (indikator CPU beku/lancar)

// Aktifasi Relay Wokwi: Sinyal HIGH menghubungkan COM (3.3V) ke NO menyalakan LED hijau
#define RELAY_ACTIVE_LEVEL   HIGH
#define RELAY_INACTIVE_LEVEL LOW

// ==================== BATAS AMBANG (THRESHOLDS) ====================
// Suhu dalam derajat Celcius
#define TEMP_THRESHOLD_HIGH     30.0f  // Jika suhu >= 30°C, exhaust fan otomatis ON
#define TEMP_HYSTERESIS         1.5f   // Kipas mati jika suhu turun ke <= (30.0 - 1.5) = 28.5°C

// Kualitas Udara (PPM gas / CO2 dari sensor MQ-135)
#define AQI_THRESHOLD_POOR      350    // Ambang batas udara buruk / tercemar
#define AQI_HYSTERESIS          30     // Kipas mati jika PPM turun ke <= (350 - 30) = 320

// Interval pembacaan sensor dan pengiriman telemetry (milidetik)
#define SENSOR_READ_INTERVAL_MS 2000   // Pembacaan sensor setiap 2 detik
#define TELEMETRY_PUBLISH_MS    2000   // Publikasi ke Dashboard setiap 2 detik

#endif // CONFIG_H
