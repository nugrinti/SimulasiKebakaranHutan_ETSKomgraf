#include "menu.h"
#include "src/ui/ui.h"
#include "../../../src/algo/bresenham.h"
#include "../../../src/algo/midcircle.h"
#include "raylib.h"

// =============================================================================
// Layout menu — layar 1000x700
// =============================================================================
#define SW  1000
#define SH   700
#define BTN_W  280
#define BTN_H   55
#define BTN_X  ((SW - BTN_W) / 2)   // tengah horizontal

// Posisi Y tiap tombol
#define BTN_SIM_Y   280
#define BTN_ELM_Y   355
#define BTN_HLP_Y   430
#define BTN_ABT_Y   505

// Warna tema
#define COL_BG      (Color){15,  30,  20,  255}   // hijau sangat gelap
#define COL_BTN1    (Color){34,  85,  34,  255}   // hijau hutan
#define COL_BTN2    (Color){20,  60,  100, 255}   // biru
#define COL_BTN3    (Color){80,  60,  20,  255}   // coklat
#define COL_BTN4    (Color){50,  50,  50,  255}   // abu
#define COL_BORDER  (Color){120, 200, 100, 255}   // hijau terang
#define COL_TITLE   (Color){200, 240, 160, 255}   // kuning-hijau

// =============================================================================
// Animasi dekoratif — titik-titik pohon kecil bergerak di background
// =============================================================================
#define DECO_COUNT 18
static float decoX[DECO_COUNT];
static float decoY[DECO_COUNT];
static float decoSpd[DECO_COUNT];
static int   decoR[DECO_COUNT];

void MenuInit(void) {
    for (int i = 0; i < DECO_COUNT; i++) {
        decoX[i]   = (float)GetRandomValue(0, SW);
        decoY[i]   = (float)GetRandomValue(0, SH);
        decoSpd[i] = (float)GetRandomValue(10, 35);
        decoR[i]   = GetRandomValue(6, 18);
    }
}

// =============================================================================
// MenuUpdate — gambar menu + tangkap input, kembalikan screen tujuan
// =============================================================================
ScreenType MenuUpdate(void) {
    float dt = GetFrameTime();

    // Update dekorasi pohon melayang
    for (int i = 0; i < DECO_COUNT; i++) {
        decoY[i] -= decoSpd[i] * dt;
        if (decoY[i] < -30) {
            decoY[i] = (float)(SH + 10);
            decoX[i] = (float)GetRandomValue(0, SW);
        }
    }

    // ----------------------------------------------------------------
    // DRAW
    // ----------------------------------------------------------------
    BeginDrawing();
    ClearBackground(COL_BG);

    // Dekorasi background — lingkaran pohon kecil melayang (Midcircle repo)
    for (int i = 0; i < DECO_COUNT; i++) {
        unsigned char alpha = 40 + (unsigned char)(decoR[i] * 3);
        Color dCol = (Color){50, 120, 50, alpha};
        Midcircle((int)decoX[i], (int)decoY[i], decoR[i], dCol);
        // Batang kecil — BresenhamLine repo
        BresenhamLine((int)decoX[i], (int)decoY[i],
                      (int)decoX[i], (int)decoY[i] + decoR[i] + 4,
                      (Color){80, 50, 20, alpha});
    }

    // Garis dekoratif atas & bawah — BresenhamLine repo
    for (int off = 0; off < 3; off++) {
        BresenhamLine(0, 70 + off, SW, 70 + off, (Color){80, 160, 60, 120});
        BresenhamLine(0, SH - 50 + off, SW, SH - 50 + off, (Color){80, 160, 60, 80});
    }

    // Judul
    const char *title    = "SIMULASI KEBAKARAN HUTAN";
    const char *subtitle = "Forest Fire Spread Simulation";
    int tw = MeasureText(title, 32);
    int sw = MeasureText(subtitle, 16);
    DrawText(title,    (SW - tw) / 2, 110, 32, COL_TITLE);
    DrawText(subtitle, (SW - sw) / 2, 152, 16, (Color){150, 200, 120, 200});

    // Garis bawah judul
    BresenhamLine((SW - tw) / 2, 178, (SW + tw) / 2, 178,
                  (Color){120, 200, 80, 180});

    // Label bawah layar
    DrawText("Grafika Komputer 2D  |  Raylib + C",
             (SW - MeasureText("Grafika Komputer 2D  |  Raylib + C", 12)) / 2,
             SH - 30, 12, (Color){100, 140, 80, 180});

    // ----------------------------------------------------------------
    // Tombol navigasi
    // ----------------------------------------------------------------
    if (DrawButton(BTN_X, BTN_SIM_Y, BTN_W, BTN_H,
                   COL_BTN1, COL_BORDER, "Mulai Simulasi", WHITE)) {
        EndDrawing();
        return SCREEN_SIMULASI;
    }

    if (DrawButton(BTN_X, BTN_ELM_Y, BTN_W, BTN_H,
                   COL_BTN2, COL_BORDER, "Elemen Simulasi", WHITE)) {
        EndDrawing();
        return SCREEN_ELEMEN;
    }

    if (DrawButton(BTN_X, BTN_HLP_Y, BTN_W, BTN_H,
                   COL_BTN3, COL_BORDER, "Cara Penggunaan", WHITE)) {
        EndDrawing();
        return SCREEN_HELP;
    }

    if (DrawButton(BTN_X, BTN_ABT_Y, BTN_W, BTN_H,
                   COL_BTN4, COL_BORDER, "Tentang", WHITE)) {
        EndDrawing();
        return SCREEN_ABOUT;
    }

    EndDrawing();
    return SCREEN_MENU;   // tetap di menu
}
