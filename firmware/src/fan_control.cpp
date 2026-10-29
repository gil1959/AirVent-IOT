#include "fan_control.h"
#include "config.h"

static FanStatus currentStatus = {
    .mode = FAN_MODE_AUTO,
    .state = FAN_STATE_OFF,
    .triggerReason = "Inisialisasi Awal"
};

void initFanControl() {
    Serial.println("[FAN] Menginisialisasi modul Relay Exhaust Fan...");
    pinMode(PIN_RELAY_FAN, OUTPUT);
    
    // Matikan kipas pada saat startup
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
        Serial.printf("[FAN ACTION] Kipas Exhaust: MENYALA (Alasan: %s)\n", reason);
    } else {
        digitalWrite(PIN_RELAY_FAN, RELAY_INACTIVE_LEVEL);
        Serial.printf("[FAN ACTION] Kipas Exhaust: MATI (Alasan: %s)\n", reason);
    }
}

void setFanMode(FanMode mode) {
    currentStatus.mode = mode;
    if (mode == FAN_MODE_AUTO) {
        Serial.println("[FAN MODE] Beralih ke Mode OTOMATIS");
    } else {
        Serial.println("[FAN MODE] Beralih ke Mode MANUAL");
    }
}

FanStatus updateFanAutomatic(const SensorData& data) {
    // Jika dalam mode manual, logika otomatis dilewati
    if (currentStatus.mode == FAN_MODE_MANUAL) {
        return currentStatus;
    }

    bool shouldTurnOn = false;
    String reason = "";

    // Cek kondisi pemicu Kipas Nyala
    if (data.temperature >= TEMP_THRESHOLD_HIGH) {
        shouldTurnOn = true;
        reason += "Suhu Tinggi (>= " + String(TEMP_THRESHOLD_HIGH, 1) + "°C) ";
    }

    if (data.gasPpm >= AQI_THRESHOLD_POOR) {
        shouldTurnOn = true;
        if (reason.length() > 0) reason += "& ";
        reason += "Udara Buruk (>= " + String(AQI_THRESHOLD_POOR) + " PPM)";
    }

    // Evaluasi status saat ini dengan Histeresis
    if (currentStatus.state == FAN_STATE_OFF) {
        if (shouldTurnOn) {
            setFanState(FAN_STATE_ON, reason.c_str());
        }
    } else {
        // Sedang ON, periksa apakah kondisi sudah cukup aman untuk OFF
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

