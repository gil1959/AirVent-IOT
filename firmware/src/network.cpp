#include "network.h"
#include "config.h"
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

static WiFiClient espClient;
static PubSubClient mqttClient(espClient);
static unsigned long lastMqttRetry = 0;

void onMqttMessageReceived(char* topic, byte* payload, unsigned int length) {
    char message[length + 1];
    memcpy(message, payload, length);
    message[length] = '\0';

    Serial.printf("[MQTT RECV] Topik: %s | Pesan: %s\n", topic, message);

    // Parsing perintah kontrol dari Dashboard
    if (String(topic) == MQTT_TOPIC_CONTROL) {
        if (strcmp(message, "AUTO") == 0) {
            setFanMode(FAN_MODE_AUTO);
        } else if (strcmp(message, "ON") == 0) {
            setFanMode(FAN_MODE_MANUAL);
            setFanState(FAN_STATE_ON, "Perintah Manual Dashboard (ON)");
        } else if (strcmp(message, "OFF") == 0) {
            setFanMode(FAN_MODE_MANUAL);
            setFanState(FAN_STATE_OFF, "Perintah Manual Dashboard (OFF)");
        }
        
        // Kirim konfirmasi status balik ke dashboard
        publishFanStatus(getFanStatus());
    }
}

void initNetwork() {
    Serial.println("[NETWORK] Memulai koneksi WiFi...");
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 20) {
        delay(500);
        Serial.print(".");
        attempts++;
    }

    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("\n[NETWORK] WiFi Terhubung!");
        Serial.print("[NETWORK] Alamat IP: ");
        Serial.println(WiFi.localIP());
    } else {
        Serial.println("\n[NETWORK WARNING] Gagal terhubung ke WiFi. Berjalan dalam mode offline.");
    }

    // Inisialisasi MQTT
    mqttClient.setServer(MQTT_BROKER, MQTT_PORT);
    mqttClient.setCallback(onMqttMessageReceived);
}

void reconnectMqtt() {
    if (WiFi.status() != WL_CONNECTED) return;

    if (!mqttClient.connected()) {
        unsigned long now = millis();
        if (now - lastMqttRetry > 5000) {
            lastMqttRetry = now;
            Serial.print("[MQTT] Menghubungkan ke broker MQTT: ");
            Serial.println(MQTT_BROKER);

            if (mqttClient.connect(MQTT_CLIENT_ID)) {
                Serial.println("[MQTT] Berhasil terhubung ke Broker!");
                mqttClient.subscribe(MQTT_TOPIC_CONTROL);
                Serial.printf("[MQTT] Berlangganan topik: %s\n", MQTT_TOPIC_CONTROL);
            } else {
                Serial.printf("[MQTT ERROR] Gagal konek, rc=%d. Mencoba lagi nanti...\n", mqttClient.state());
            }
        }
    }
}

void handleNetworkLoop() {
    if (WiFi.status() == WL_CONNECTED) {
        if (!mqttClient.connected()) {
            reconnectMqtt();
        } else {
            mqttClient.loop();
        }
    }
}

bool isNetworkConnected() {
    return (WiFi.status() == WL_CONNECTED && mqttClient.connected());
}

void publishTelemetry(const SensorData& sensorData, const FanStatus& fanStatus) {
    if (!mqttClient.connected()) return;

    StaticJsonDocument<256> doc;
    doc["temperature"] = sensorData.temperature;
    doc["humidity"] = sensorData.humidity;
    doc["gasPpm"] = sensorData.gasPpm;
    doc["airQuality"] = sensorData.airQualityText;
    doc["fanState"] = (fanStatus.state == FAN_STATE_ON) ? "ON" : "OFF";
    doc["fanMode"] = (fanStatus.mode == FAN_MODE_AUTO) ? "AUTO" : "MANUAL";
    doc["reason"] = fanStatus.triggerReason;

    char buffer[256];
    serializeJson(doc, buffer);

    mqttClient.publish(MQTT_TOPIC_DATA, buffer);
    Serial.printf("[MQTT PUB] Telemetry terkirim ke %s\n", MQTT_TOPIC_DATA);
}

void publishFanStatus(const FanStatus& fanStatus) {
    if (!mqttClient.connected()) return;

    StaticJsonDocument<128> doc;
    doc["state"] = (fanStatus.state == FAN_STATE_ON) ? "ON" : "OFF";
    doc["mode"] = (fanStatus.mode == FAN_MODE_AUTO) ? "AUTO" : "MANUAL";
    doc["reason"] = fanStatus.triggerReason;

    char buffer[128];
    serializeJson(doc, buffer);

    mqttClient.publish(MQTT_TOPIC_STATUS, buffer);
}

