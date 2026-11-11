# Hardware AirVent

Folder ini menjadi referensi komponen dan lokasi berkas rangkaian proyek AirVent.

## Sumber rangkaian

- [diagram.json di root proyek](../diagram.json): rangkaian simulasi Wokwi, termasuk tombol, switch mode, dan LED heartbeat untuk demonstrasi.
- [Konfigurasi pin firmware](../firmware/include/config.h): definisi pin yang digunakan program.
- [Dokumentasi wiring](../docs/wiring_diagram.md): penjelasan sambungan yang sudah tersedia.

Berkas simulasi tetap berada di lokasi asal agar konfigurasi Wokwi yang ada tetap dapat digunakan.

## Komponen pada diagram root

| Komponen simulasi | Jumlah | Sambungan / fungsi |
|---|---:|---|
| ESP32 DevKit V1 | 1 | Pengendali utama |
| DS18B20 | 1 | Data ke GPIO 4, sensor suhu |
| Sensor gas | 1 | Output analog ke GPIO 34 |
| Modul relay | 1 | Input kendali dari GPIO 26 |
| LED hijau | 1 | Indikator exhaust fan melalui kontak NO relay |
| Pushbutton | 1 | GPIO 19 dan GND |
| Slide switch | 1 | GPIO 18 dan GND, pemilih mode |
| LED biru | 1 | GPIO 23 melalui resistor, heartbeat |
| Resistor 4,7 kΩ | 1 | Pull-up data DS18B20 |
| Resistor 220 Ω | 2 | Pembatas arus LED hijau dan biru |

Daftar ini mengikuti berkas simulasi yang ada, bukan laporan pengujian perangkat fisik. Rangkaian saat ini menggunakan DS18B20; penambahan folder ini tidak mengubah jenis sensor atau firmware.

## Pengembangan berikutnya

Foto perakitan, skema tambahan, dan daftar komponen perangkat fisik dapat disimpan di folder ini ketika tersedia. Dokumentasi teknis umum tetap berada di folder `docs/`.

