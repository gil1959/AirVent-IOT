# Panduan Kalibrasi Sensor MQ-135 (Air Quality Index)

Sensor gas semikonduktor seperti **MQ-135** (Tin Dioxide - $SnO_2$) membutuhkan kalibrasi resistansi awal agar pembacaan PPM (Parts Per Million) akurat dan konsisten.

---

## 1. Pemanasan Awal (Preheating / Burn-In Time)

Sensor MQ memiliki elemen pemanas internal (*heater*).
- **Sensor Baru**: Harus dipanaskan (*pre-heat*) selama minimal **24 hingga 48 jam** terus menerus terhubung ke daya 5V sebelum digunakan untuk pembacaan kritis.
- **Penggunaan Sehari-hari**: Berikan jeda sekitar **3 hingga 5 menit** setelah perangkat dinyalakan sebelum menganggap datanya valid.

---

## 2. Prinsip Kerja Resistansi Sensor

Konduktivitas sensor berubah sebanding dengan konsentrasi gas:
- $R_s$: Resistansi sensor pada konsentrasi gas saat ini.
- $R_0$: Resistansi sensor pada udara bersih (*clean air*).
- Rasio $\frac{R_s}{R_0}$ digunakan untuk mencari nilai konsentrasi gas (PPM) melalui kurva logaritmik datasheet:

$$\text{PPM} = A \times \left(\frac{R_s}{R_0}\right)^B$$

*(Nilai konstanta $A$ dan $B$ bergantung pada target gas spesifik seperti $CO_2$, $NH_3$, atau Asap).*

---

## 3. Langkah Mendapatkan Nilai $R_0$ Udara Bersih

1. Bawa alat ke ruangan berventilasi baik atau ruang terbuka luar (*outdoor*) dengan udara segar alami.
2. Ukur tegangan analog output ($V_{out}$) pada pin ADC:
   $$V_{out} = \text{ADC} \times \frac{3.3}{4095}$$
3. Hitung resistansi sensor $R_s$:
   $$R_s = \frac{(V_{in} - V_{out}) \times R_L}{V_{out}}$$
   *(di mana $R_L$ adalah load resistor pada modul, umumnya bernilai $10k\Omega$ atau $20k\Omega$).*
4. Untuk udara bersih, nilai $R_0$ dapat diaproksimasi dari rasio udara bersih datasheet:
   $$R_0 = \frac{R_s}{3.6}$$
5. Catat nilai $R_0$ tersebut dan masukkan ke dalam file `firmware/include/config.h` untuk perhitungan PPM yang presisi.

