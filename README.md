# AirVent: Sistem Exhaust Fan Otomatis Berbasis Indeks Kualitas Udara & Suhu (ESP32)

Sistem ventilasi pintar (*Smart Exhaust Fan*) berbasis Internet of Things (IoT) yang mampu mengatur sirkulasi udara secara otomatis berdasarkan parameter suhu, kelembaban, dan tingkat pencemaran/kualitas udara (gas PPM / AQI), dilengkapi dengan *dashboard monitoring real-time*.

---

## 📌 Fitur Utama
1. **Pendeteksian Kualitas Udara & Suhu**:
   - Memantau konsentrasi gas berbahaya / kualitas udara menggunakan sensor MQ-135.
   - Memantau suhu dan kelembaban ruangan menggunakan sensor DHT11/DHT22.
2. **Kontrol Otomatis Exhaust Fan**:
   - Kipas menyala otomatis ketika suhu atau kadar gas melebihi batas aman (*threshold*).
   - Fitur *hysteresis* untuk mencegah kipas mati-nyala secara berulang-ulang (*hunting*).
3. **Kontrol Manual / Override**:
   - Pengguna dapat mematikan atau menyalakan kipas secara manual melalui web dashboard.
4. **Monitoring Real-Time**:
   - Dashboard antarmuka web interaktif menampilkan grafik riwayat data, status kipas, dan peringatan kualitas udara.
5. **Konektivitas Fleksibel**:
   - Pengiriman data via protokol MQTT atau WebSockets/HTTP REST.

---

## 🗂️ Struktur Direktori Proyek

```text
airvent/
├── firmware/                       # Source code mikroprosesor ESP32
│   ├── platformio.ini              # Konfigurasi PlatformIO & library dependency
│   ├── include/
│   │   ├── config.h               # Pinout, WiFi credentials, broker & batas threshold
│   │   ├── sensors.h              # Definisi fungsi sensor (DHT & MQ-135)
│   │   ├── fan_control.h          # Definisi logika kendali exhaust fan
│   │   └── network.h              # Definisi komunikasi WiFi & MQTT/WebSockets
│   └── src/
│       ├── main.cpp               # Alur program utama ESP32 (setup & loop)
│       ├── sensors.cpp            # Pembacaan analog/digital & konversi PPM
│       ├── fan_control.cpp        # Logika aktuator Relay/PWM
│       └── network.cpp            # Pengiriman telemetry data & penerimaan perintah
├── dashboard/                     # Web Dashboard Monitoring Real-Time
│   ├── index.html                 # Halaman utama visualisasi data
│   ├── css/
│   │   └── style.css              # Styling kustom dashboard
│   └── js/
│       └── app.js                 # Logika interaktif data, grafik, & kontrol fan
├── docs/                          # Dokumentasi Teknis
│   ├── wiring_diagram.md          # Skema sambungan pin (pinout wiring)
│   └── calibration_guide.md       # Panduan kalibrasi sensor gas MQ-135
└── .gitignore
```

---

## 🛠️ Kebutuhan Hardware
| Komponen | Spesifikasi / Tipe | Catatan |
|---|---|---|
| Microcontroller | ESP32 DevKit V1 (30 / 38 pin) | Menggunakan koneksi WiFi 2.4 GHz |
| Sensor Suhu & Kelembaban | DHT22 / DHT11 | DHT22 lebih akurat untuk pembacaan suhu |
| Sensor Kualitas Udara | MQ-135 | Mendeteksi CO2, Asap, Alkohol, Benzena, NH3 |
| Aktuator | Relay 5V Module (1 Channel) | Saklar penghubung exhaust fan |
| Kipas | Exhaust Fan (12V DC / 220V AC) | Disesuaikan dengan kebutuhan ruangan |
| Sumber Daya | Power Supply 12V DC + Buck Converter 5V | Suplai terpisah untuk fan & ESP32 |

---

## 🚀 Panduan Memulai Cepat

### 1. Persiapan Firmware (ESP32)
1. Install **VS Code** dan ekstensi **PlatformIO IDE**.
2. Buka folder `firmware/`.
3. Buka file `include/config.h`:
   - Sesuaikan `WIFI_SSID` dan `WIFI_PASSWORD`.
   - Sesuaikan `MQTT_BROKER` dan topik jika menggunakan MQTT.
   - Atur batas *threshold* suhu dan kualitas udara sesuai kebutuhan.
4. Hubungkan ESP32 ke laptop/PC dengan kabel data micro-USB / Type-C.
5. Klik **Upload** di toolbar PlatformIO.

### 2. Menjalankan Web Dashboard
1. Buka file `dashboard/index.html` langsung di browser atau gunakan ekstensi **Live Server** di VS Code.
2. Dashboard akan langsung menampilkan simulasi data atau menerima data dari ESP32.

