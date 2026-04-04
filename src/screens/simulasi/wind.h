#ifndef WIND_H
#define WIND_H

// -----------------------------------------------------------------------------
// Modul angin:
//   - Visualisasi garis angin bergerak di layar
//   - Kompas interaktif untuk mengubah arah angin (drag mouse)
//   - Getter arah & kecepatan angin untuk dipakai GridUpdate()
// -----------------------------------------------------------------------------

// Inisialisasi sistem angin
void WindInit(void);

// Update posisi partikel garis angin + baca input kompas
// cx, cy = posisi tengah kompas di layar
void WindUpdate(float dt, int compassCX, int compassCY);

// Gambar semua elemen angin (garis + kompas)
void WindDraw(int compassCX, int compassCY);

// Getter — dipakai oleh fire_sim.c untuk dikirim ke GridUpdate()
float WindGetDirX(void);    // komponen X arah angin (-1 .. 1)
float WindGetDirY(void);    // komponen Y arah angin (-1 .. 1)
float WindGetSpeed(void);   // kecepatan angin (0 .. 1)
void  WindSetSpeed(float s); // dipanggil dari slider di fire_sim.c

#endif // WIND_H
