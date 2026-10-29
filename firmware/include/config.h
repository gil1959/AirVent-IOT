#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

// ==================== KONFIGURASI WIFI ====================
#define WIFI_SSID           "NAMA_WIFI_ANDA"
#define WIFI_PASSWORD       "PASSWORD_WIFI_ANDA"

// ==================== KONFIGURASI MQTT BROKER ====================
// Contoh broker publik (dapat diganti EMQX, HiveMQ, Mosquitto lokal, atau Adafruit IO)
#define MQTT_BROKER         "broker.emqx.io"
#define MQTT_PORT           1883
#define MQTT_CLIENT_ID      "AirVent-ESP32-Client"
#define MQTT_TOPIC_DATA     "airvent/telemetry"       // Topik publikasi sensor
#define MQTT_TOPIC_STATUS   "airvent/fan/status"      // Topik status kipas
#define MQTT_TOPIC_CONTROL  "airvent/fan/control"     // Topik kontrol manual (ON/OFF/AUTO)

// ==================== KONFIGURASI PINOUT GPIO ====================
#define PIN_DHT             4        // GPIO4 untuk DHT11 / DHT22 Data
#define DHT_TYPE            DHT22    // DHT11 atau DHT22

#define PIN_MQ135           34       // GPIO34 (ADC1) untuk pin Analog MQ-135
#define PIN_RELAY_FAN       18       // GPIO18 untuk sinyal Relay Exhaust Fan
#define PIN_LED_STATUS      2        // GPIO2 untuk LED indikator onboard ESP32

// Aktifasi Relay: kebanyakan modul relay aktif LOW (LOW = Relay ON)
#define RELAY_ACTIVE_LEVEL  LOW
#define RELAY_INACTIVE_LEVEL HIGH

// ==================== BATAS AMBANG (THRESHOLDS) ====================
// Suhu dalam derajat Celcius
#define TEMP_THRESHOLD_HIGH     30.0f  // Jika suhu > 30°C, exhaust fan otomatis ON
#define TEMP_HYSTERESIS         1.5f   // Kipas mati jika suhu turun ke (30.0 - 1.5) = 28.5°C

// Kualitas Udara (PPM perkiraan gas / CO2)
#define AQI_THRESHOLD_POOR      350    // Di atas batas ini, udara dianggap tercemar/kurang sehat
#define AQI_HYSTERESIS          30     // Kipas mati jika PPM turun ke (350 - 30) = 320

// Interval pembacaan sensor dan publikasi telemetry (dalam milidetik)
#define SENSOR_READ_INTERVAL_MS 2000   // Baca sensor setiap 2 detik
#define TELEMETRY_PUBLISH_MS    3000   // Kirim telemetry setiap 3 detik

#endif // CONFIG_H

