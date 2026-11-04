#include "airvent_net.h"
#include "config.h"
#include "fan_control.h"
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

    // Parsing perintah kontrol dari Web Dashboard
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
        
        // Kirim konfirmasi status balik seketika ke dashboard
        publishFanStatus(getFanStatus());
    }
}

void initNetwork() {
    Serial.println("[NETWORK] Memulai koneksi WiFi simulasi (Wokwi-GUEST)...");
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD, 6);

    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 25) {
        delay(300);
        Serial.print(".");
        attempts++;
    }

    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("\n[NETWORK] WiFi Terhubung!");
        Serial.print("[NETWORK] Alamat IP: ");
        Serial.println(WiFi.localIP());
    } else {
        Serial.println("\n[NETWORK WARNING] Belum terhubung WiFi, loop akan mencoba terus.");
    }

    // Inisialisasi MQTT
    mqttClient.setServer(MQTT_BROKER, MQTT_PORT);
    mqttClient.setCallback(onMqttMessageReceived);
    mqttClient.setBufferSize(512); // Buffer cukup untuk payload JSON
}

void reconnectMqtt() {
    if (WiFi.status() != WL_CONNECTED) return;

    if (!mqttClient.connected()) {
        unsigned long now = millis();
        if (now - lastMqttRetry > 3000) {
            lastMqttRetry = now;
            Serial.printf("[MQTT] Menghubungkan ke %s:%d ...\n", MQTT_BROKER, MQTT_PORT);

            // Client ID acak untuk mencegah tabrakan di broker publik
            String clientId = String(MQTT_CLIENT_ID) + "-" + String(random(1000, 9999));
            if (mqttClient.connect(clientId.c_str())) {
                Serial.println("[MQTT] Berhasil terhubung ke Broker EMQX!");
                mqttClient.subscribe(MQTT_TOPIC_CONTROL);
                Serial.printf("[MQTT] Berlangganan topik kontrol: %s\n", MQTT_TOPIC_CONTROL);
            } else {
                Serial.printf("[MQTT ERROR] Gagal konek, rc=%d. Coba lagi...\n", mqttClient.state());
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

    StaticJsonDocument<300> doc;
    doc["temperature"] = round(sensorData.temperature * 10.0f) / 10.0f;
    doc["temp"] = round(sensorData.temperature * 10.0f) / 10.0f;
    doc["humidity"] = round(sensorData.humidity * 10.0f) / 10.0f;
    doc["gasPpm"] = round(sensorData.gasPpm);
    doc["idx"] = map(sensorData.rawGasValue, 0, 4095, 0, 100);
    doc["airQuality"] = sensorData.airQualityText;
    doc["fanState"] = (fanStatus.state == FAN_STATE_ON) ? "ON" : "OFF";
    doc["fan"] = (fanStatus.state == FAN_STATE_ON) ? 1 : 0;
    doc["fanMode"] = (fanStatus.mode == FAN_MODE_AUTO) ? "AUTO" : "MANUAL";
    doc["reason"] = fanStatus.triggerReason;

    char buffer[300];
    serializeJson(doc, buffer);

    mqttClient.publish(MQTT_TOPIC_DATA, buffer);
    Serial.printf("[MQTT PUB -> %s] %s\n", MQTT_TOPIC_DATA, buffer);
}

void publishFanStatus(const FanStatus& fanStatus) {
    if (!mqttClient.connected()) return;

    StaticJsonDocument<150> doc;
    doc["fanState"] = (fanStatus.state == FAN_STATE_ON) ? "ON" : "OFF";
    doc["state"] = (fanStatus.state == FAN_STATE_ON) ? "ON" : "OFF";
    doc["fan"] = (fanStatus.state == FAN_STATE_ON) ? 1 : 0;
    doc["fanMode"] = (fanStatus.mode == FAN_MODE_AUTO) ? "AUTO" : "MANUAL";
    doc["mode"] = (fanStatus.mode == FAN_MODE_AUTO) ? "AUTO" : "MANUAL";
    doc["reason"] = fanStatus.triggerReason;

    char buffer[150];
    serializeJson(doc, buffer);

    mqttClient.publish(MQTT_TOPIC_STATUS, buffer);
    Serial.printf("[MQTT PUB -> %s] %s\n", MQTT_TOPIC_STATUS, buffer);
}

