# Panduan Demonstrasi: Perbedaan Respons Tombol (Blocking vs Non-Blocking)

Dokumen ini adalah panduan lengkap untuk mendemonstrasikan kriteria penilaian **Rubrik No. 3**:
> *"Mahasiswa mampu mendemonstrasikan perbedaan respons tombol antara mode blocking dan non-blocking."*

---

## 🎯 Ringkasan Konsep Teknis

| Parameter | Mode BLOCKING (`delay`) | Mode NON-BLOCKING (`millis`) |
|---|---|---|
| **Fungsi Waktu** | `delay(3000)` | `millis()` timer |
| **Status Prosesor (CPU)** | **Membeku / Freeze** selama jeda waktu. | **Aktif & Multitasking** setiap saat. |
| **Respons Tombol Manual** | ❌ **Sangat Buruk / Terabaikan**: Tombol yang ditekan saat delay tidak terdeteksi atau baru merespons setelah jeda selesai. | ✅ **Instan (< 5 milidetik)**: Tombol langsung direspons kapan pun ditekan tanpa menunggu sensor selesai dibaca. |
| **Indikator Visual LED Heartbeat** | ❌ **Membeku / Mati Total** selama 3 detik. | ✅ **Berkedip Stabil & Lancar** setiap 250ms. |
| **Koneksi Jaringan (MQTT)** | Rawan terputus (*connection timeout*). | Stabil menjaga koneksi *keep-alive*. |

---

## 🕹️ Komponen Demo di Simulasi Wokwi

1. **Push Button Merah (GPIO 19)**: Tombol manual untuk menguji responsivitas (toggle relay/kipas).
2. **Slide Switch Mode (GPIO 18)**:
   - **Posisi Kiri (LOW)** = Mode **BLOCKING**
   - **Posisi Kanan (HIGH)** = Mode **NON-BLOCKING**
3. **LED Biru Heartbeat (GPIO 23)**: Indikator visual aktivitas CPU mikrokontroler.
4. **Relay & LED Hijau Fan (GPIO 26)**: Menunjukkan status exhaust fan (ON/OFF).
5. **Serial Monitor**: Bisa juga mengetik `'b'` (Blocking) atau `'n'` (Non-Blocking).

---

## 📋 Langkah-Langkah Demonstrasi di Depan Dosen

### Tahap 1: Demonstrasikan Mode BLOCKING (Sistem Bermasalah)

1. **Aktifkan Mode Blocking**:
   - Geser **Slide Switch ke Kiri**, ATAU ketik huruf `b` lalu tekan Enter di Serial Monitor.
   - Serial Monitor akan mencetak: `⚠️ MODE AKTIF: [ BLOCKING (delay) ]`.
2. **Perhatikan Indikator Heartbeat**:
   - Tunjukkan ke dosen bahwa **LED Biru berhenti berkedip (membeku)** karena CPU tertahan di `delay(3000)`.
3. **Uji Respons Tombol**:
   - Minta dosen menekan **Push Button Merah**.
   - **Hasil Nyata**:
     - Jika tombol ditekan singkat lalu dilepas saat delay berlangsung, **kipas/relay TIDAK merespons sama sekali (input hilang)**.
     - Jika tombol ditekan tahan, kipas baru merespons setelah menunggu jeda 3 detik selesai.
4. **Penjelasan ke Dosen**:
   > *"Pak/Bu, pada mode blocking dengan fungsi `delay()`, mikrokontroler membeku total. Ketika pengguna menekan tombol darurat atau ingin mematikan kipas, perintah tersebut diabaikan oleh prosesor."*

---

### Tahap 2: Demonstrasikan Mode NON-BLOCKING (Sistem Proyek AirVent)

1. **Beralih ke Mode Non-Blocking**:
   - Geser **Slide Switch ke Kanan**, ATAU ketik huruf `n` di Serial Monitor.
   - Serial Monitor akan mencetak: `✅ MODE AKTIF: [ NON-BLOCKING (millis) ]`.
2. **Perhatikan Indikator Heartbeat**:
   - Tunjukkan ke dosen bahwa **LED Biru langsung berkedip lancar setiap 250ms**.
3. **Uji Respons Tombol**:
   - Tekan **Push Button Merah** kapan saja.
   - **Hasil Nyata**:
     - Kipas (Relay & LED Hijau) **LANGSUNG berganti status seketika (< 5ms)** setiap kali tombol diklik!
     - Pembacaan sensor suhu & gas tetap berjalan normal di latar belakang tanpa terganggu.
4. **Penjelasan ke Dosen**:
   > *"Pada mode non-blocking, kami menggunakan fungsi penanda waktu `millis()`. ESP32 tetap memeriksa status tombol dan komunikasi jaringan pada setiap milidetik siklus CPU. Hasilnya, respons tombol manual menjadi instan dan sistem tetap aman merespons kebocoran gas secara real-time."*

