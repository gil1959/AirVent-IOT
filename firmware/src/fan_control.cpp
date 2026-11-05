#include "fan_control.h"
#include "config.h"

static FanStatus currentStatus = {
    .mode = FAN_MODE_AUTO,
    .state = FAN_STATE_OFF,
    .triggerReason = "Inisialisasi Sistem"
};

void initFanControl() {
    Serial.println("[FAN] Menginisialisasi Relay Exhaust Fan (GPIO 26)...");
    pinMode(PIN_RELAY_FAN, OUTPUT);
    digitalWrite(PIN_RELAY_FAN, RELAY_INACTIVE_LEVEL);
    
    currentStatus.state = FAN_STATE_OFF;
    currentStatus.mode = FAN_MODE_AUTO;
    currentStatus.triggerReason = "Sistem Siap";
}

void setFanState(FanState state, const char* reason) {
    currentStatus.state = state;
    currentStatus.triggerReason = reason;

    if (state == FAN_STATE_ON) {
        digitalWrite(PIN_RELAY_FAN, RELAY_ACTIVE_LEVEL);
        Serial.printf("[FAN ACTION] Kipas / Relay: MENYALA (Alasan: %s)\n", reason);
    } else {
        digitalWrite(PIN_RELAY_FAN, RELAY_INACTIVE_LEVEL);
        Serial.printf("[FAN ACTION] Kipas / Relay: MATI (Alasan: %s)\n", reason);
    }
}

void setFanMode(FanMode mode) {
    currentStatus.mode = mode;
    if (mode == FAN_MODE_AUTO) {
        currentStatus.triggerReason = "Beralih ke Mode Otomatis";
        Serial.println("[FAN MODE] Beralih ke Mode OTOMATIS");
    } else {
        currentStatus.triggerReason = "Beralih ke Mode MANUAL";
        Serial.println("[FAN MODE] Beralih ke Mode MANUAL");
    }
}

void toggleFanState(const char* reason) {
    setFanMode(FAN_MODE_MANUAL);
    if (currentStatus.state == FAN_STATE_ON) {
        setFanState(FAN_STATE_OFF, reason);
    } else {
        setFanState(FAN_STATE_ON, reason);
    }
}

FanStatus updateFanAutomatic(const SensorData& data) {
    // Abaikan otomatisasi jika dalam mode manual (override pengguna)
    if (currentStatus.mode == FAN_MODE_MANUAL) {
        return currentStatus;
    }

    bool shouldTurnOn = false;
    String reason = "";

    // Pemicu ON
    if (data.temperature >= TEMP_THRESHOLD_HIGH) {
        shouldTurnOn = true;
        reason += "Suhu Tinggi (>= " + String(TEMP_THRESHOLD_HIGH, 1) + "°C) ";
    }

    if (data.gasPpm >= AQI_THRESHOLD_POOR) {
        shouldTurnOn = true;
        if (reason.length() > 0) reason += "& ";
        reason += "Gas Berbahaya (>= " + String(AQI_THRESHOLD_POOR) + " PPM)";
    }

    // Logika Histeresis
    if (currentStatus.state == FAN_STATE_OFF) {
        if (shouldTurnOn) {
            setFanState(FAN_STATE_ON, reason.c_str());
        }
    } else {
        // Sedang ON: matikan hanya jika kedua parameter sudah turun di bawah batas aman
        bool tempSafe = data.temperature <= (TEMP_THRESHOLD_HIGH - TEMP_HYSTERESIS);
        bool gasSafe = data.gasPpm <= (AQI_THRESHOLD_POOR - AQI_HYSTERESIS);

        if (tempSafe && gasSafe) {
            setFanState(FAN_STATE_OFF, "Parameter Suhu & Udara Normal");
        }
    }

    return currentStatus;
}

FanStatus getFanStatus() {
    return currentStatus;
}
