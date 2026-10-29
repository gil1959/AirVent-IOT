# Panduan Skema Rangkaian (Wiring Diagram) AirVent

Dokumen ini menjelaskan koneksi perkabelan antara **ESP32**, **Sensor Suhu (DHT22/DHT11)**, **Sensor Gas Kualitas Udara (MQ-135)**, **Modul Relay**, dan **Exhaust Fan**.

---

## 📌 Tabel Pemetaan Pin (Pinout Mapping)

| Komponen | Pin Komponen | Pin ESP32 | Keterangan Tegangan |
|---|---|---|---|
| **Sensor DHT22 / DHT11** | VCC | 3.3V / 5V | Hubungkan ke 3.3V ESP32 (disarankan DHT22) |
| | GND | GND | Ground bersama (Common Ground) |
| | DATA | **GPIO 4** | Pasang pull-up resistor 4.7kΩ - 10kΩ ke VCC jika modul belum memilikinya |
| **Sensor MQ-135** | VCC | 5V (VIN) | MQ-135 membutuhkan pemanas internal 5V |
| | GND | GND | Ground bersama |
| | A0 (Analog Out) | **GPIO 34** | Pin ADC1 ESP32 (Hanya input analog) |
| | D0 (Digital Out)| *Tidak digunakan*| Bisa dibiarkan kosong |
| **Modul Relay 5V** | VCC | 5V (VIN) | Suplai koil relay |
| | GND | GND | Ground bersama |
| | IN (Sinyal) | **GPIO 18** | Mengendalikan saklar relay (Active LOW) |
| **LED Indikator Onboard** | - | **GPIO 2** | Internal LED status sistem |

---

## ⚡ Skema Jalur Daya & Exhaust Fan

> [!CAUTION]
> **PENTING: JANGAN MENGHUBUNGKAN DAYA EXHAUST FAN LANGSUNG DARI PIN ESP32!**
> ESP32 hanya mampu mengeluarkan arus maksimal ~12mA per GPIO dan bekerja di 3.3V. Exhaust fan biasanya membutuhkan tegangan 12V DC (0.5A - 2A) atau 220V AC.

### 1. Jika Menggunakan Exhaust Fan 12V DC
```text
[ Power Supply 12V DC ]
   (+) 12V -------------> [ Relay COM ]
   (-) GND -------------> [ Fan (-) Ground ]
                          [ Relay NO ] ---------> [ Fan (+) 12V ]

   * Ground Power Supply 12V dan Ground ESP32 harus dihubungkan bersama (Common Ground).
   * Gunakan DC-DC Buck Converter (Step-Down LM2596) untuk menurunkan 12V ke 5V guna menyalakan ESP32 via pin VIN.
```

### 2. Jika Menggunakan Exhaust Fan 220V AC (Listrik PLN)
```text
[ Sumber Listrik PLN 220V ]
   Phase (L) -----------> [ Relay COM ]
   Neutral (N) ---------> [ Fan Terminal 1 ]
                          [ Relay NO ] ---------> [ Fan Terminal 2 ]

* Selalu pastikan isolasi kabel listrik PLN aman untuk menghindari sengatan listrik.
```

---

## 💡 Tips & Catatan Hardware
1. **Pull-Up Resistor DHT**: Kebanyakan modul DHT yang sudah berada di PCB breakout sudah memiliki resistor pull-up bawaan. Jika menggunakan sensor lepasan (4 kaki), tambahkan resistor 10kΩ antara pin Data dan VCC.
2. **Karakteristik ADC ESP32**: Pin ADC ESP32 memiliki rentang tegangan masukan **0V - 3.3V** (resolusi 12-bit: 0 - 4095). Pastikan tegangan output analog dari sensor tidak melebihi 3.3V saat membaca gas konsentrasi tinggi (bisa gunakan pembagi tegangan *voltage divider* jika perlu).

