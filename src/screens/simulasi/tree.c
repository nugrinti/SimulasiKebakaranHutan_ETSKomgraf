#include "tree.h"
#include "src/algo/bresenham.h"   // BresenhamLine()
#include "src/algo/midcircle.h"   // Midcircle(), MidcircleFilled()
#include "raylib.h"
#include <math.h>

// =============================================================================
// Helper internal: scan-fill segitiga (isi warna solid tanpa DrawTriangle)
// Menggunakan BresenhamLine() horizontal per baris — sesuai aturan manual
// =============================================================================
//   (tx, ty)          = puncak segitiga
//   (blx,bly),(brx,bry) = pojok kiri-bawah dan kanan-bawah
static void FillTriangle(int tx, int ty,
                          int blx, int bly,
                          int brx, int bry,
                          Color col)
{
    // Iterasi dari y puncak (atas) ke y bawah, hitung batas kiri-kanan tiap baris
    int yTop = ty;
    int yBot = bly;   // bly == bry karena alas horizontal

    for (int y = yTop; y <= yBot; y++) {
        // Interpolasi linear batas kiri & kanan
        float t = (yBot == yTop) ? 1.0f : (float)(y - yTop) / (float)(yBot - yTop);
        int xLeft  = (int)(tx + t * (blx - tx));
        int xRight = (int)(tx + t * (brx - tx));
        // Gambar garis horizontal 1 baris → isi segitiga
        BresenhamLine(xLeft, y, xRight, y, col);
    }
}

// =============================================================================
// Helper internal: isi persegi panjang (batang pohon) secara manual
// Loop BresenhamLine() horizontal per baris
// =============================================================================
static void FillRect(int x1, int y1, int x2, int y2, Color col) {
    for (int y = y1; y <= y2; y++) {
        BresenhamLine(x1, y, x2, y, col);
    }
}

// =============================================================================
// TreeInit
// =============================================================================
void TreeInit(Tree *t, int sx, int sy, int trunkW, int trunkH, int crownSize) {
    t->x          = sx;
    t->y          = sy;
    t->trunkW     = trunkW;
    t->trunkH     = trunkH;
    t->crownSize  = crownSize;
    t->state      = TREE_HEALTHY;
    t->burnTimer  = 3.0f;   // terbakar selama 3 detik lalu hangus
}

// =============================================================================
// TreeUpdate — panggil tiap frame dengan delta time
// =============================================================================
void TreeUpdate(Tree *t, float dt) {
    if (t->state == TREE_BURNING) {
        t->burnTimer -= dt;
        if (t->burnTimer <= 0.0f) {
            t->state     = TREE_BURNED;
            t->burnTimer = 0.0f;
        }
    }
}

// =============================================================================
// TreeDraw — gambar pohon sesuai state
//
// Koordinat referensi:
//   (t->x, t->y)              = tengah bawah batang
//   batang   : lebar trunkW, tinggi trunkH  → ke atas dari t->y
//   mahkota  : 3 segitiga berlapis di atas batang
//   bayangan : ellips pipih di bawah batang
// =============================================================================
void TreeDraw(const Tree *t, float time, float windSpeed) {

    // Pohon bergoyang saat angin kencang (nomor 2)
    // Offset horizontal berdasarkan sin(time) * windSpeed
    int windOffset = (int)(sinf(time * 2.5f + t->x * 0.05f) * windSpeed * 4.0f);

    int cx  = t->x + windOffset;   // ← cx bergeser sesuai angin
    int by  = t->y;
    int ty  = by - t->trunkH;
    int hw  = t->trunkW / 2;

    // ------------------------------------------------------------------
    // POHON SEHAT — hijau
    // ------------------------------------------------------------------
    if (t->state == TREE_HEALTHY) {

        // 1. Bayangan — ellips pipih di tanah
        //    Gunakan Midcircle dari repo, tapi gambar berulang dengan
        //    offset Y dikompres (skala Y = 0.3) untuk efek ellips
        //    → karena Midcircle hanya bisa lingkaran, kita scan manual
        int shadowRx = t->crownSize;          // radius horizontal bayangan
        int shadowRy = t->crownSize / 4;      // radius vertikal (pipih)
        Color shadowCol = (Color){0, 0, 0, 60};
        for (int dy = -shadowRy; dy <= shadowRy; dy++) {
            // rumus ellips: (dx/rx)^2 + (dy/ry)^2 <= 1
            float ratio = (float)dy / shadowRy;
            int dx = (int)(shadowRx * sqrtf(1.0f - ratio * ratio));
            BresenhamLine(cx - dx, by + dy, cx + dx, by + dy, shadowCol);
        }

        // 2. Batang — coklat, isi dengan FillRect
        Color trunkCol = (Color){101, 67, 33, 255};
        FillRect(cx - hw, ty, cx + hw, by, trunkCol);

        // 3. Outline batang — gelap, pakai BresenhamLine (dari repo)
        Color trunkOutline = (Color){60, 30, 10, 255};
        BresenhamLine(cx - hw, ty, cx - hw, by, trunkOutline); // kiri
        BresenhamLine(cx + hw, ty, cx + hw, by, trunkOutline); // kanan

        // 4. Mahkota — 3 segitiga berlapis (bawah → atas, makin kecil)
        //    Layer 1 (paling bawah, paling lebar)
        int cs = t->crownSize;

        int seed = (cx * 7 + by * 13) % 40;
        Color darkGreen  = (Color){(unsigned char)(25 + seed/2), (unsigned char)(70 + seed),  (unsigned char)(20 + seed/3), 255};
        Color green      = (Color){(unsigned char)(40 + seed/2), (unsigned char)(105 + seed), (unsigned char)(35 + seed/3), 255};
        Color lightGreen = (Color){(unsigned char)(70 + seed/2), (unsigned char)(145 + seed), (unsigned char)(50 + seed/3), 255};

        // Layer 1
        FillTriangle(cx,            ty - cs,
                     cx - cs,       ty,
                     cx + cs,       ty,
                     darkGreen);
        BresenhamLine(cx - cs, ty,       cx,       ty - cs, darkGreen);
        BresenhamLine(cx,      ty - cs,  cx + cs,  ty,      darkGreen);

        // Layer 2 (tengah)
        int cs2 = (int)(cs * 0.75f);
        int mid2y = ty - (int)(cs * 0.4f);
        FillTriangle(cx,              mid2y - cs2,
                     cx - cs2,        mid2y,
                     cx + cs2,        mid2y,
                     green);

        // Layer 3 (puncak, paling kecil)
        int cs3 = (int)(cs * 0.5f);
        int mid3y = ty - (int)(cs * 0.75f);
        FillTriangle(cx,              mid3y - cs3,
                     cx - cs3,        mid3y,
                     cx + cs3,        mid3y,
                     lightGreen);
    }

    // ------------------------------------------------------------------
    // POHON TERBAKAR — merah/oranye + partikel sederhana
    // ------------------------------------------------------------------
    else if (t->state == TREE_BURNING) {

        // Batang — coklat gelap
        FillRect(cx - hw, ty, cx + hw, by, (Color){80, 40, 10, 255});

        // Mahkota terbakar — oranye kemerahan
        int cs = t->crownSize;
        Color fireOrange = (Color){220, 100, 20, 255};
        Color fireRed    = (Color){180, 40,  10, 255};
        Color fireYellow = (Color){255, 200, 50, 255};

        FillTriangle(cx,       ty - cs,
                     cx - cs,  ty,
                     cx + cs,  ty,
                     fireOrange);

        int cs2 = (int)(cs * 0.7f);
        int mid2y = ty - (int)(cs * 0.4f);
        FillTriangle(cx,        mid2y - cs2,
                     cx - cs2,  mid2y,
                     cx + cs2,  mid2y,
                     fireRed);

        // "Lidah api" — beberapa garis DDA pendek ke atas dari puncak
        int tipY = ty - cs;
        // Offset api bergerak: sin(time) untuk goyang kiri-kanan
        int fx = (int)(sinf(time * 6.0f + t->x * 0.1f) * 3.0f);
        int fy = (int)(sinf(time * 8.0f + t->x * 0.1f) * 2.0f);  // naik-turun sedikit

        BresenhamLine(cx,         tipY,        cx + fx,      tipY - 8  + fy, fireYellow);
        BresenhamLine(cx - 3,     tipY - 2,    cx - 3 + fx,  tipY - 6  + fy, fireOrange);
        BresenhamLine(cx + 3,     tipY - 2,    cx + 3 + fx,  tipY - 6  + fy, fireOrange);
        BresenhamLine(cx - 6,     tipY,        cx - 4 + fx,  tipY - 4  + fy, fireRed);
        BresenhamLine(cx + 6,     tipY,        cx + 4 + fx,  tipY - 4  + fy, fireRed);
    }
    // ------------------------------------------------------------------
    // POHON HANGUS — hanya batang hitam + sisa abu
    // ------------------------------------------------------------------
    else if (t->state == TREE_BURNED) {

        // Batang hangus — hitam
        Color charcoal = (Color){30, 30, 30, 255};
        FillRect(cx - hw, ty, cx + hw, by, charcoal);

        // Outline batang
        BresenhamLine(cx - hw, ty, cx - hw, by, (Color){50, 50, 50, 255});
        BresenhamLine(cx + hw, ty, cx + hw, by, (Color){50, 50, 50, 255});

        // Sisa abu — segitiga kecil di atas batang, abu-abu gelap
        int cs = t->crownSize / 3;
        Color ashColor = (Color){80, 80, 80, 200};
        FillTriangle(cx,       ty - cs,
                     cx - cs,  ty,
                     cx + cs,  ty,
                     ashColor);

        // Beberapa titik abu beterbangan (DrawPixel langsung — paling dasar)
        Color ashDot = (Color){120, 120, 120, 150};
        DrawPixel(cx - 5, ty - cs - 3, ashDot);
        DrawPixel(cx + 4, ty - cs - 5, ashDot);
        DrawPixel(cx,     ty - cs - 7, ashDot);
    }
}
