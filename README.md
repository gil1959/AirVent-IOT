
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
├── diagram.json                    # Skema rangkaian simulator Wokwi
├── wokwi.toml                      # Konfigurasi binary firmware Wokwi
├── firmware/                       # Source code mikroprosesor ESP32
│   ├── diagram.json                # Skema Wokwi untuk folder firmware
│   ├── wokwi.toml                  # Konfigurasi Wokwi untuk folder firmware
│   ├── platformio.ini              # Konfigurasi PlatformIO & library dependency
│   ├── include/
│   │   ├── config.h               # Pinout (DS18B20 GPIO4, MQ-135 GPIO34, Relay GPIO26)
│   │   ├── sensors.h              # Definisi fungsi sensor DS18B20 & MQ-135
│   │   ├── fan_control.h          # Definisi logika kendali exhaust fan & histeresis
│   │   └── network.h              # Komunikasi WiFi & MQTT (PubSubClient)
│   └── src/
│       ├── main.cpp               # Alur program utama ESP32 (setup & loop)
│       ├── sensors.cpp            # Pembacaan DS18B20 OneWire & konversi PPM MQ-135
│       ├── fan_control.cpp        # Logika aktuator Relay/LED indikator
│       └── network.cpp            # Pengiriman telemetry real-time & penerima perintah
├── dashboard/                     # Web Dashboard Monitoring Real-Time
│   ├── index.html                 # Halaman utama visualisasi data & kontrol
│   ├── css/
│   │   └── style.css              # Styling kustom dashboard
│   └── js/
│       └── app.js                 # Logika interaktif data, grafik, & kontrol fan via MQTT WS
├── docs/                          # Dokumentasi Teknis
│   ├── wiring_diagram.md          # Skema sambungan pin (pinout wiring)
│   └── calibration_guide.md       # Panduan kalibrasi sensor gas MQ-135
└── .gitignore
```

---

## 🛠️ Kebutuhan Hardware & Simulasi
| Komponen | Pin ESP32 | Keterangan / Komponen Wokwi |
|---|---|---|
| Microcontroller | - | ESP32 DevKit V1 (WiFi: `Wokwi-GUEST`) |
| Sensor Suhu | **GPIO 4** | `wokwi-ds18b20` (OneWire + Resistor 4.7kΩ) |
| Sensor Kualitas Udara | **GPIO 34** | `wokwi-gas-sensor` (MQ-135 Analog A0) |
| Aktuator Relay / Fan | **GPIO 26** | `wokwi-relay-module` (Active HIGH) |
| Indikator Exhaust Fan | Output Relay NO | `wokwi-led` (Hijau + Resistor 220Ω) |
| Broker MQTT | - | `broker.emqx.io` (TCP 1883 / WSS 8084) |

---

## 🚀 Panduan Menjalankan Simulasi Wokwi & Web Dashboard

### 1. Menjalankan Simulasi Wokwi di VS Code
1. Pastikan ekstensi **Wokwi Simulator** telah terpasang di VS Code.
2. Buka berkas `diagram.json`.
3. Tekan tombol **Play / Start Simulation** (atau tekan `F1` lalu ketik `Wokwi: Start Simulator`).
4. ESP32 akan langsung terhubung ke WiFi `Wokwi-GUEST` dan terhubung ke broker MQTT `broker.emqx.io`.
5. Anda dapat mengklik sensor **DS18B20** atau **Gas Sensor** di Wokwi untuk menggeser nilai slider suhu dan kadar gas.

### 2. Menjalankan & Menghubungkan Web Dashboard
1. Buka berkas `dashboard/index.html` langsung di browser (atau via ekstensi **Live Server**).
2. Di pojok kanan atas navbar, klik tombol **"Beralih ke Live MQTT"**.
3. Status koneksi akan berubah menjadi **"Live MQTT Terhubung (Wokwi / ESP32)"**.
4. Grafik dan kartu metrik (Suhu, Gas PPM, dan Status Fan) akan langsung ter-update secara *real-time* mengikuti perubahan slider di Wokwi!
5. Anda juga dapat menguji tombol **Fan ON**, **Fan OFF**, atau **Auto** di dashboard — relay dan lampu LED hijau di simulator Wokwi akan langsung merespons seketika.

