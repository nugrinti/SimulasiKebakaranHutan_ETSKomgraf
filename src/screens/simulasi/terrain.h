#ifndef TERRAIN_H
#define TERRAIN_H

#include "raylib.h"

// -----------------------------------------------------------------------------
// Ukuran layar — sesuaikan jika berubah
// -----------------------------------------------------------------------------
#define SCREEN_W  1000
#define SCREEN_H   700

// -----------------------------------------------------------------------------
// Zona layar (posisi Y penting untuk tata letak)
// -----------------------------------------------------------------------------
//  0          ← atas layar
//  HILL_Y     ← puncak bukit tertinggi (~200)
//  GROUND_Y   ← batas tanah/rumput (~480)
//  SCREEN_H   ← bawah layar (700)

#define HILL_Y    200
#define GROUND_Y  480

// -----------------------------------------------------------------------------
// API publik
// -----------------------------------------------------------------------------

// Gambar semua elemen terrain (urutan: bukit → tanah → sungai)
void TerrainDraw(void);

// Kembalikan 1 jika titik (x,y) berada di atas sungai (untuk cek firebreak)
// int TerrainIsRiver(int x, int y);

#endif // TERRAIN_H
