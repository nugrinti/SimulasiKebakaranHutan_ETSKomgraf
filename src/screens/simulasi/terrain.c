#include "terrain.h"
#include "src/algo/bresenham.h"   // BresenhamLine()
#include "raylib.h"
#include <math.h>

// =============================================================================
// Titik-titik sungai berkelok
// Ini adalah array koordinat (x,y) layar yang membentuk jalur sungai.
// Sungai mengalir dari kiri-tengah ke kanan-bawah membelah hutan.
// Semakin banyak titik = semakin berkelok = nilai koordinat lebih tinggi.
// =============================================================================
// static int riverX[] = { 120, 160, 210, 270, 310, 360, 400, 460, 510, 560, 610, 670, 720, 780, 840, 900, 960 };
// static int riverY[] = { 390, 410, 395, 420, 405, 430, 415, 440, 420, 445, 425, 450, 430, 445, 435, 450, 440 };
// #define RIVER_POINTS 17
// #define RIVER_WIDTH   6   // lebar sungai dalam piksel (digambar berlapis)

// =============================================================================
// Helper: scan-fill segitiga untuk bukit
// Sama persis logikanya dengan FillTriangle di tree.c,
// dibuat ulang di sini agar terrain.c tidak bergantung ke tree.c
// Pakai BresenhamLine horizontal per baris
// =============================================================================
static void FillTriangleTerrain(int tx, int ty,
                                 int blx, int bly,
                                 int brx, int bry,
                                 Color col)
{
    int yTop = ty;
    int yBot = bly;
    for (int y = yTop; y <= yBot; y++) {
        float t    = (yBot == yTop) ? 1.0f : (float)(y - yTop) / (float)(yBot - yTop);
        int xLeft  = (int)(tx + t * (blx - tx));
        int xRight = (int)(tx + t * (brx - tx));
        BresenhamLine(xLeft, y, xRight, y, col);
    }
}

// =============================================================================
// Helper: scan-fill persegi panjang (untuk tanah & rumput)
// Loop BresenhamLine horizontal dari y1 ke y2
// =============================================================================
static void FillRectTerrain(int x1, int y1, int x2, int y2, Color col) {
    for (int y = y1; y <= y2; y++) {
        BresenhamLine(x1, y, x2, y, col);
    }
}

// =============================================================================
// DrawHills — bukit berlapis di latar belakang
//
// Strategi: 3 bukit dengan posisi & ukuran berbeda, warna makin terang
// ke depan (atmospheric perspective sederhana)
//
// Bukit digambar sebagai segitiga besar menggunakan FillTriangleTerrain()
// Urutan: belakang (gelap) → depan (terang) agar tumpuk dengan benar
// =============================================================================
static void DrawHills(void) {

    // --- Bukit paling belakang (kiri) ---
    Color hill3 = (Color){50, 90, 50, 255};
    FillTriangleTerrain(
        200, HILL_Y - 100,           // puncak
        -40, GROUND_Y - 200,              // pojok kiri bawah
        440, GROUND_Y - 200,              // pojok kanan bawah
        hill3
    );

    // --- Bukit paling belakang (kanan) ---
    FillTriangleTerrain(
        780, HILL_Y - 150,
        540, GROUND_Y - 150,
        1040, GROUND_Y - 400,
        hill3
    );

    // --- Bukit tengah (sedikit lebih depan) ---
    Color hill2 = (Color){65, 110, 55, 255};
    FillTriangleTerrain(
        500, HILL_Y - 120,                // puncak lebih tinggi
        220, GROUND_Y - 120,
        780, GROUND_Y - 120,
        hill2
    );

    // --- Bukit paling kanan ---
    Color hill1 = (Color){65, 110, 55, 255};
    FillTriangleTerrain(
        500, HILL_Y - 120,                // puncak lebih tinggi
        220, GROUND_Y - 120,
        780, GROUND_Y - 120,
        hill1
    );

    // Tutup celah antara bukit dan area pohon dengan warna hijau gelap
    FillRectTerrain(0, GROUND_Y - 200, SCREEN_W, GROUND_Y, (Color){45, 90, 45, 255});

    // --- Outline puncak bukit — BresenhamLine (dari repo) ---
    Color hillEdge = (Color){40, 75, 40, 255};
    BresenhamLine(200, HILL_Y + 20, -40,  GROUND_Y - 100, hillEdge);
    BresenhamLine(200, HILL_Y + 20, 440,  GROUND_Y - 100, hillEdge);
    BresenhamLine(780, HILL_Y + 10, 540,  GROUND_Y - 90, hillEdge);
    BresenhamLine(780, HILL_Y + 10, 1040, GROUND_Y - 90, hillEdge);
    BresenhamLine(500, HILL_Y - 120,      220,  GROUND_Y - 120, hillEdge);
    BresenhamLine(500, HILL_Y - 120,      780,  GROUND_Y - 120, hillEdge);
}

// =============================================================================
// DrawGround — tanah dan lapisan rumput di atasnya
//
// Lapisan rumput : GROUND_Y sampai GROUND_Y+12  → hijau
// Lapisan tanah  : GROUND_Y+12 sampai SCREEN_H  → coklat
// Keduanya pakai FillRectTerrain (loop BresenhamLine horizontal)
// =============================================================================
static void DrawGround(void) {
    // Tanah coklat
    Color soil = (Color){110, 75, 35, 255};
    FillRectTerrain(0, GROUND_Y + 12, SCREEN_W, SCREEN_H, soil);

    // Lapisan rumput hijau di atas tanah
    Color grassDark  = (Color){80,  140, 50, 255};
    Color grassLight = (Color){100, 160, 60, 255};
    // Baris 0-5 rumput gelap
    FillRectTerrain(0, GROUND_Y,     SCREEN_W, GROUND_Y + 5,  grassDark);
    // Baris 6-12 rumput terang
    FillRectTerrain(0, GROUND_Y + 6, SCREEN_W, GROUND_Y + 12, grassLight);
}

// =============================================================================
// DrawRiver — sungai berkelok dari array titik
//
// Cara menggambar:
//   Untuk setiap pasang titik berurutan [i] → [i+1],
//   gambar BresenhamLine() sebanyak RIVER_WIDTH kali dengan offset Y
//   agar sungai tampak punya lebar.
//
// Warna: biru gelap (dasar) + biru terang (highlight tengah)
// =============================================================================
// static void DrawRiver(void) {
//     Color riverDeep    = (Color){50,  100, 180, 220};
//     Color riverShallow = (Color){80,  150, 220, 200};
//     Color riverLight   = (Color){120, 180, 240, 180};

//     for (int i = 0; i < RIVER_POINTS - 1; i++) {
//         int x1 = riverX[i],     y1 = riverY[i];
//         int x2 = riverX[i + 1], y2 = riverY[i + 1];

//         // Gambar berlapis: offset -2 sampai +2 dari garis tengah
//         for (int off = -RIVER_WIDTH / 2; off <= RIVER_WIDTH / 2; off++) {
//             Color col;
//             if (off == 0 || off == 1)      col = riverLight;    // tengah terang
//             else if (abs(off) <= 2)        col = riverShallow;  // tengah
//             else                           col = riverDeep;      // tepi

//             BresenhamLine(x1, y1 + off, x2, y2 + off, col);
//         }
//     }

//     // Outline tepi sungai — garis gelap di atas dan bawah
//     Color riverEdge = (Color){30, 70, 140, 255};
//     for (int i = 0; i < RIVER_POINTS - 1; i++) {
//         // Tepi atas
//         BresenhamLine(riverX[i], riverY[i] - RIVER_WIDTH / 2,
//                       riverX[i+1], riverY[i+1] - RIVER_WIDTH / 2,
//                       riverEdge);
//         // Tepi bawah
//         BresenhamLine(riverX[i], riverY[i] + RIVER_WIDTH / 2,
//                       riverX[i+1], riverY[i+1] + RIVER_WIDTH / 2,
//                       riverEdge);
//     }
// }

// =============================================================================
// TerrainDraw 
// =============================================================================
void TerrainDraw(void) {
    DrawHills();
    DrawGround();
    // DrawRiver();
}

// =============================================================================
// TerrainIsRiver — cek apakah titik (x,y) berada di atas jalur sungai
// Dipakai oleh GridUpdate untuk mencegah api menyeberang sungai
//
// Cara: cek apakah (x,y) dekat dengan salah satu segmen sungai
// Menggunakan jarak titik ke garis sederhana
// =============================================================================
// int TerrainIsRiver(int x, int y) {
//     for (int i = 0; i < RIVER_POINTS - 1; i++) {
//         int x1 = riverX[i],     y1 = riverY[i];
//         int x2 = riverX[i + 1], y2 = riverY[i + 1];

//         // Vektor segmen
//         float dx = (float)(x2 - x1);
//         float dy = (float)(y2 - y1);
//         float len2 = dx*dx + dy*dy;
//         if (len2 == 0.0f) continue;

//         // Proyeksi titik ke segmen
//         float t = ((x - x1)*dx + (y - y1)*dy) / len2;
//         if (t < 0.0f) t = 0.0f;
//         if (t > 1.0f) t = 1.0f;

//         float px = x1 + t*dx;
//         float py = y1 + t*dy;

//         // Jarak titik ke segmen
//         float dist = sqrtf((x - px)*(x - px) + (y - py)*(y - py));
//         if (dist <= (float)(RIVER_WIDTH / 2 + 2)) return 1;
//     }
//     return 0;
// }
