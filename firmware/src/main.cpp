#include <Arduino.h>
#include "config.h"
#include "sensors.h"
#include "fan_control.h"
#include "airvent_net.h"

// Timer Non-Blocking
static unsigned long lastSensorRead = 0;
static unsigned long lastTelemetryPublish = 0;
static unsigned long lastHeartbeat = 0;
static unsigned long lastButtonPress = 0;

static SensorData currentSensorData;
static FanStatus currentFanStatus;

// State Demo Blocking vs Non-Blocking
static bool isBlockingMode = false;
static bool heartbeatState = false;
static int lastButtonState = HIGH;

void printDemoStatus() {
    Serial.println("\n--------------------------------------------------");
    if (isBlockingMode) {
        Serial.println("⚠️  MODE AKTIF SAAT INI: [ BLOCKING (delay) ]");
        Serial.println("    Perilaku: CPU akan tertahan delay(3000ms).");
        Serial.println("    Uji Coba: Coba klik Push Button (GPIO 19) sekarang!");
        Serial.println("    Hasil   : Tombol TIDAK MERESPONS atau sangat lambat,");
        Serial.println("              dan LED Heartbeat berhenti berkedip (Freeze).");
    } else {
        Serial.println("✅ MODE AKTIF SAAT INI: [ NON-BLOCKING (millis) ]");
        Serial.println("    Perilaku: CPU tidak pernah berhenti.");
        Serial.println("    Uji Coba: Klik Push Button (GPIO 19) kapan saja!");
        Serial.println("    Hasil   : Kipas & Relay merespons INSTAN (<5ms),");
        Serial.println("              dan LED Heartbeat berkedip lancar.");
    }
    Serial.println("--------------------------------------------------\n");
}

void setup() {
    Serial.begin(115200);
    delay(500);
    Serial.println("\n=======================================================");
    Serial.println("     AirVent: Sistem Exhaust Fan Otomatis ESP32        ");
    Serial.println("   DEMO RESPON TOMBOL: BLOCKING vs NON-BLOCKING        ");
    Serial.println("=======================================================");

    // Inisialisasi Pin Output & Input
    pinMode(PIN_LED_STATUS, OUTPUT);
    digitalWrite(PIN_LED_STATUS, LOW);

    pinMode(PIN_LED_HEARTBEAT, OUTPUT);
    digitalWrite(PIN_LED_HEARTBEAT, LOW);

    // Push Button & Slide Switch (menggunakan internal pullup)
    pinMode(PIN_BUTTON_TEST, INPUT_PULLUP);
    pinMode(PIN_SWITCH_MODE, INPUT_PULLUP);

    // Inisialisasi Modul
    initFanControl();
    initSensors();
    initNetwork();

    digitalWrite(PIN_LED_STATUS, HIGH); // LED indikator setup selesai

    Serial.println("\n📌 PANDUAN PENGUJIAN UNTUK DOSEN:");
    Serial.println("   1. Ketik 'b' di Serial Monitor untuk beralih ke MODE BLOCKING.");
    Serial.println("   2. Ketik 'n' di Serial Monitor untuk beralih ke MODE NON-BLOCKING.");
    Serial.println("   3. Atau geser Slide Switch di Wokwi (GPIO 18).");
    Serial.println("   4. Tekan Push Button Merah (GPIO 19) untuk melihat perbedaan respons!");

    // Cek posisi awal switch (LOW = Blocking, HIGH = Non-blocking)
    isBlockingMode = (digitalRead(PIN_SWITCH_MODE) == LOW);
    printDemoStatus();
}

void checkModeSwitch() {
    // 1. Cek input karakter dari Serial Monitor
    if (Serial.available() > 0) {
        char cmd = Serial.read();
        if (cmd == 'b' || cmd == 'B') {
            if (!isBlockingMode) {
                isBlockingMode = true;
                printDemoStatus();
            }
        } else if (cmd == 'n' || cmd == 'N') {
            if (isBlockingMode) {
                isBlockingMode = false;
                printDemoStatus();
            }
        }
    }

    // 2. Cek posisi hardware switch (jika digeser di Wokwi)
    static int lastSwitchState = -1;
    int currentSwitchState = digitalRead(PIN_SWITCH_MODE);
    if (currentSwitchState != lastSwitchState) {
        lastSwitchState = currentSwitchState;
        bool newMode = (currentSwitchState == LOW);
        if (newMode != isBlockingMode) {
            isBlockingMode = newMode;
            printDemoStatus();
        }
    }
}

void loop() {
    checkModeSwitch();

    // ==============================================================
    // SKENARIO 1: DEMO MODE BLOCKING (Menggunakan delay)
    // ==============================================================
    if (isBlockingMode) {
        Serial.println("\n⏳ [BLOCKING ACTIVE] CPU masuk delay(3000ms)...");
        Serial.println("   >> TEKAN TOMBOL PUSH-BUTTON SEKARANG! <<");

        // Lampu heartbeat dipaksa menyala terus (membeku, indikator CPU terkunci)
        digitalWrite(PIN_LED_HEARTBEAT, HIGH);

        // ❌ BLOCKING: Sistem mematung selama 3 detik!
        delay(3000);

        digitalWrite(PIN_LED_HEARTBEAT, LOW);

        // Tombol baru dicek SETELAH delay selesai
        int btn = digitalRead(PIN_BUTTON_TEST);
        if (btn == LOW) {
            toggleFanState("Tombol Manual (Mode Blocking)");
            publishFanStatus(getFanStatus());
            Serial.println("⚠️  [HASIL TOMBOL]: Tombol baru merespons SETELAH jeda 3 detik selesai (Sangat Lambat)!");
        } else {
            Serial.println("❌ [HASIL TOMBOL]: Jika tadi Anda mengklik tombol saat delay, klik tersebut TERLEWATKAN / HILANG!");
        }

        // Jalankan tugas jaringan & sensor sekilas
        handleNetworkLoop();
        currentSensorData = readSensors();
        currentFanStatus = updateFanAutomatic(currentSensorData);
        publishTelemetry(currentSensorData, currentFanStatus);
        return;
    }

    // ==============================================================
    // SKENARIO 2: DEMO MODE NON-BLOCKING (Menggunakan millis)
    // ==============================================================
    unsigned long currentMillis = millis();

    // 1. LED Heartbeat berkedip lancar setiap 250ms (Bukti CPU tidak pernah freeze)
    if (currentMillis - lastHeartbeat >= 250) {
        lastHeartbeat = currentMillis;
        heartbeatState = !heartbeatState;
        digitalWrite(PIN_LED_HEARTBEAT, heartbeatState);
    }

    // 2. Pembacaan Tombol Non-Blocking (Diperiksa setiap milidetik)
    int currentBtnState = digitalRead(PIN_BUTTON_TEST);
    if (currentBtnState == LOW && lastButtonState == HIGH) {
        // Debounce non-blocking 150ms
        if (currentMillis - lastButtonPress >= 150) {
            lastButtonPress = currentMillis;
            toggleFanState("Tombol Manual (Mode Non-Blocking)");
            publishFanStatus(getFanStatus());

            Serial.println("\n⚡ ========================================================");
            Serial.println("⚡ [HASIL TOMBOL NON-BLOCKING]: RESPON INSTAN (<5 milidetik)!");
            Serial.println("⚡ Kipas & Relay langsung berubah seketika tanpa delay!");
            Serial.println("⚡ ========================================================\n");
        }
    }
    lastButtonState = currentBtnState;

    // 3. Jaringan WiFi & MQTT jalan terus tanpa terhalang
    handleNetworkLoop();

    // 4. Pembacaan Sensor berkala (setiap 2 detik) TANPA delay()
    if (currentMillis - lastSensorRead >= SENSOR_READ_INTERVAL_MS) {
        lastSensorRead = currentMillis;
        currentSensorData = readSensors();

        if (currentSensorData.isValid) {
            Serial.printf("[SENSOR DATA] Suhu: %.1f°C | Gas: %.1f PPM | Status: %s\n",
                          currentSensorData.temperature,
                          currentSensorData.gasPpm,
                          currentSensorData.airQualityText);
        }

        currentFanStatus = updateFanAutomatic(currentSensorData);
    }

    // 5. Kirim Telemetry berkala (setiap 2 detik) TANPA delay()
    if (currentMillis - lastTelemetryPublish >= TELEMETRY_PUBLISH_MS) {
        lastTelemetryPublish = currentMillis;
        if (currentSensorData.isValid) {
            publishTelemetry(currentSensorData, currentFanStatus);
        }
    }
}
