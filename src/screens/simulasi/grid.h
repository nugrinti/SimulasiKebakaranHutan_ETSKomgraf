#ifndef GRID_H
#define GRID_H

#include "tree.h"

// -----------------------------------------------------------------------------
// Ukuran grid — bisa disesuaikan
// -----------------------------------------------------------------------------
#define GRID_COLS  12
#define GRID_ROWS   8

// -----------------------------------------------------------------------------
// Struktur grid
// -----------------------------------------------------------------------------
typedef struct {
    Tree trees[GRID_ROWS][GRID_COLS];
    int  cellW;    // lebar tiap sel (px)
    int  cellH;    // tinggi tiap sel (px)
    int  originX;  // pojok kiri atas area grid (layar)
    int  originY;  // pojok kiri atas area grid (layar)
} ForestGrid;

// Inisialisasi seluruh grid pohon
void GridInit(ForestGrid *g, int originX, int originY, int cellW, int cellH);

// Update semua pohon + logika penyebaran api
// windDirX, windDirY : vektor arah angin (-1..1), windSpeed : 0..1
void GridUpdate(ForestGrid *g, float dt, float windDirX, float windDirY, float windSpeed);

// Gambar semua pohon dalam grid
void GridDraw(const ForestGrid *g, float time, float windSpeed);

// Picu api pada pohon di sel (col, row) — dipanggil saat user klik
void GridIgnite(ForestGrid *g, int col, int row);

// Reset seluruh grid ke kondisi sehat
void GridReset(ForestGrid *g);

// Konversi posisi mouse (mx, my) ke kolom & baris grid
// Mengembalikan 1 jika dalam batas grid, 0 jika di luar
int GridPickCell(const ForestGrid *g, int mx, int my, int *outCol, int *outRow);

#endif 
