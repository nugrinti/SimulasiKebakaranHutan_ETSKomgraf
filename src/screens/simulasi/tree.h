#ifndef TREE_H
#define TREE_H

#include "raylib.h"

// -----------------------------------------------------------------------------
// State pohon — urutan fase kebakaran
// -----------------------------------------------------------------------------
typedef enum {
    TREE_HEALTHY  = 0,   // hijau normal
    TREE_BURNING  = 1,   // sedang terbakar (punya timer)
    TREE_BURNED   = 2    // sudah hangus
} TreeState;

// -----------------------------------------------------------------------------
// Struktur data satu pohon
// -----------------------------------------------------------------------------
typedef struct {
    int x;               // posisi layar tengah bawah batang (px)
    int y;               // posisi layar bawah batang (px)
    int trunkW;          // lebar batang (px), misal 6
    int trunkH;          // tinggi batang (px), misal 20
    int crownSize;       // "radius" mahkota / setengah alas segitiga (px)
    TreeState state;
    float burnTimer;     // detik tersisa sebelum jadi BURNED (misal 3.0f)
} Tree;

// -----------------------------------------------------------------------------
// API publik
// -----------------------------------------------------------------------------

// Inisialisasi pohon di posisi (sx, sy) layar
void TreeInit(Tree *t, int sx, int sy, int trunkW, int trunkH, int crownSize);

// Update logika (hitung burnTimer setiap frame)
void TreeUpdate(Tree *t, float dt);

// Gambar pohon sesuai state-nya
void TreeDraw(const Tree *t, float time, float windSpeed);

#endif // TREE_H
