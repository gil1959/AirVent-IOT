# Pemeriksaan Repositori AirVent

Folder ini berisi panduan pemeriksaan manual untuk poin 4 praktikum: struktur lima folder utama dan riwayat commit anggota tim.

## 1. Periksa struktur folder

Jalankan dari root repositori:

```sh
git ls-tree -d --name-only HEAD
```

Lima folder utama yang diharapkan:

```text
dashboard
docs
firmware
hardware
tests
```

Buka setiap folder di GitHub dan pastikan berisi berkas yang sesuai fungsinya. Git melacak berkas, sehingga folder baru dilengkapi README yang menjelaskan kegunaannya.

## 2. Periksa riwayat kontribusi

```sh
git log --format="%h | %an | %ae | %s"
git shortlog -sne HEAD
```

Bandingkan daftar author dengan daftar anggota kelompok. Di GitHub, buka riwayat commit dan periksa apakah commit masing-masing anggota terhubung ke akun yang benar.

Nama atau email author pada Git perlu terhubung ke akun GitHub anggota tersebut. Membuat folder atau menulis nama anggota dalam dokumen saja belum menjadi bukti commit anggota itu.

## 3. Kriteria pemeriksaan

- [ ] Lima folder utama tersedia dan memiliki isi.
- [ ] Struktur pada README utama sesuai dengan berkas di repositori.
- [ ] Tautan dokumentasi dapat dibuka.
- [ ] Setiap anggota kelompok memiliki commit kontribusinya sendiri.
- [ ] Identitas author pada commit sesuai dengan akun anggota.

Checklist di atas adalah panduan verifikasi, bukan pernyataan bahwa seluruh anggota sudah memenuhi persyaratan.

## Batas pemeriksaan

Penambahan dokumentasi ini tidak membuktikan keberhasilan build, simulasi, atau pengujian perangkat keras. Pengujian fungsional mengikuti [panduan proyek](../README.md) dan [panduan demonstrasi yang tersedia](../docs/demo_blocking_nonblocking.md).
