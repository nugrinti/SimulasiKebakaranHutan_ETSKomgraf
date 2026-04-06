#include "terrain.h"
#include "src/algo/bresenham.h"   // BresenhamLine()
#include "raylib.h"
#include <math.h>

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

static void FillRectTerrain(int x1, int y1, int x2, int y2, Color col) {
    for (int y = y1; y <= y2; y++) {
        BresenhamLine(x1, y, x2, y, col);
    }
}

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
// TerrainDraw 
// =============================================================================
void TerrainDraw(void) {
    DrawHills();
    DrawGround();
    // DrawRiver();
}

