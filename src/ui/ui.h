#ifndef UI_H
#define UI_H

#include "raylib.h"

// -----------------------------------------------------------------------------
// Komponen UI manual — semua menggunakan BresenhamLine() dari repo
// Dipakai bersama oleh menu, about, help, elemen, fire_simulation
// -----------------------------------------------------------------------------

// Gambar tombol persegi dengan label teks di tengah
// Mengembalikan 1 jika mouse hover di atasnya
int  DrawButton(int x, int y, int w, int h,
                Color fill, Color border,
                const char *label, Color textCol);

// Cek apakah mouse berada di dalam area (x,y,w,h)
int  IsMouseOver(int x, int y, int w, int h);

// Gambar slider horizontal, nilai 0..1
// Mengembalikan nilai baru jika user drag
float DrawSlider(int x, int y, int w, float value,
                 Color track, Color thumb, const char *label);

// Scan-fill persegi panjang dengan BresenhamLine horizontal
void FillRectUI(int x1, int y1, int x2, int y2, Color col);

// Gambar garis teks wrapping sederhana (potong per \n)
void DrawTextBlock(const char *text, int x, int y, int fontSize, Color col);

#endif // UI_H
