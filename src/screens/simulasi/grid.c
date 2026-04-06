#include "grid.h"
#include "tree.h"
#include "particle.h"
#include <math.h>
#include <stdlib.h>   // rand()

// =============================================================================
// GridInit
// =============================================================================
void GridInit(ForestGrid *g, int originX, int originY, int cellW, int cellH) {
    g->originX = originX;
    g->originY = originY;
    g->cellW   = cellW;
    g->cellH   = cellH;

    for (int r = 0; r < GRID_ROWS; r++) {
        for (int c = 0; c < GRID_COLS; c++) {
            // Posisi tengah bawah pohon dalam sel
            int sx = originX + c * cellW + cellW / 2 + GetRandomValue(-30, 8);
            int sy = originY + r * cellH + cellH - 4 + GetRandomValue(-4, 30);
            int trunkH = cellH/3 + GetRandomValue(-3,4);
            int crownSize = cellW/3 + GetRandomValue(-3, 4);
            TreeInit(&g->trees[r][c], sx, sy, 6, trunkH, crownSize);
        }
    }
}

// =============================================================================
// GridUpdate
// Logika penyebaran api — cellular automaton sederhana:
//   Setiap pohon BURNING punya peluang menyalakan tetangganya tiap frame.
//   Angin meningkatkan peluang ke arah tertentu.
// =============================================================================
void GridUpdate(ForestGrid *g, float dt,
                float windDirX, float windDirY, float windSpeed)
{
    // Tandai pohon yang akan terbakar di frame ini
    int willIgnite[GRID_ROWS][GRID_COLS] = {0};

    for (int r = 0; r < GRID_ROWS; r++) {
        for (int c = 0; c < GRID_COLS; c++) {
            TreeUpdate(&g->trees[r][c], dt);

            if (g->trees[r][c].state != TREE_BURNING) continue;

            // Cek 4 tetangga: atas, bawah, kiri, kanan
            int dr[] = {-1,  1,  0,  0};
            int dc[] = { 0,  0, -1,  1};
            // Arah vektor layar: baris bertambah ke bawah, kolom ke kanan
            // windDirX positif = angin ke kanan, windDirY positif = angin ke bawah
            float windDot[] = {
                -windDirY,   // tetangga atas   (dr=-1): searah angin jika angin ke atas
                 windDirY,   // tetangga bawah  (dr=+1)
                -windDirX,   // tetangga kiri
                 windDirX    // tetangga kanan
            };

            for (int i = 0; i < 4; i++) {
                int nr = r + dr[i];
                int nc = c + dc[i];
                if (nr < 0 || nr >= GRID_ROWS) continue;
                if (nc < 0 || nc >= GRID_COLS) continue;
                if (g->trees[nr][nc].state != TREE_HEALTHY) continue;

                // Probabilitas dasar penyebaran per detik = 0.4
                // Angin menambah probabilitas hingga +0.4 ke arah angin
                float baseProb = 0.4f;
                float windBonus = windDot[i] * windSpeed * 0.4f;
                float prob = baseProb + windBonus;
                if (prob < 0.05f) prob = 0.05f;  // minimal selalu ada peluang

                // Konversi probabilitas per-detik ke per-frame
                float frameProb = 1.0f - pow(1.0f - prob, dt);
                float r01 = (float)rand() / (float)RAND_MAX;
                if (r01 < frameProb) {
                    willIgnite[nr][nc] = 1;
                }
            }
        }
    }

    // Terapkan ignition + spawn asap
    for (int r = 0; r < GRID_ROWS; r++) {
        for (int c = 0; c < GRID_COLS; c++) {
            // Ignition
            if (willIgnite[r][c]) {
                g->trees[r][c].state     = TREE_BURNING;
                g->trees[r][c].burnTimer = 2.5f + (float)(rand() % 30) * 0.1f;
            }
            // Spawn asap untuk pohon yang terbakar
            if (g->trees[r][c].state == TREE_BURNING) {
                if (GetRandomValue(0, 8) == 0) {
                    int tx = g->trees[r][c].x;
                    int ty = g->trees[r][c].y - g->trees[r][c].trunkH
                             - g->trees[r][c].crownSize;
                    ParticleSpawn(tx, ty);
                }
            }
        }
    }
}

// =============================================================================
// GridDraw
// =============================================================================
void GridDraw(const ForestGrid *g, float time, float windSpeed) {
    for (int r = 0; r < GRID_ROWS; r++) {
        for (int c = 0; c < GRID_COLS; c++) {
            TreeDraw(&g->trees[r][c], time, windSpeed);
        }
    }
}

// =============================================================================
// GridIgnite — picu api pada 1 sel
// =============================================================================
void GridIgnite(ForestGrid *g, int col, int row) {
    if (row < 0 || row >= GRID_ROWS) return;
    if (col < 0 || col >= GRID_COLS) return;
    if (g->trees[row][col].state != TREE_HEALTHY) return;

    g->trees[row][col].state     = TREE_BURNING;
    g->trees[row][col].burnTimer = 3.0f;
}

// =============================================================================
// GridReset
// =============================================================================
void GridReset(ForestGrid *g) {
    GridInit(g, g->originX, g->originY, g->cellW, g->cellH);
}

// =============================================================================
// GridPickCell — konversi posisi mouse ke sel grid
// =============================================================================
int GridPickCell(const ForestGrid *g, int mx, int my, int *outCol, int *outRow) {
    int relX = mx - g->originX;
    int relY = my - g->originY;

    if (relX < 0 || relY < 0) return 0;

    int col = relX / g->cellW;
    int row = relY / g->cellH;

    if (col >= GRID_COLS || row >= GRID_ROWS) return 0;

    *outCol = col;
    *outRow = row;
    return 1;
}
