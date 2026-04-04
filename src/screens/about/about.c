#include "about.h"
#include "src/ui/ui.h"
#include "../../../src/algo/bresenham.h"
#include "../../../src/algo/midcircle.h"
#include "raylib.h"

#define SW 1000
#define SH  700

ScreenType AboutUpdate(void) {
    BeginDrawing();
    ClearBackground((Color){15, 25, 35, 255});

    // Header
    FillRectUI(0, 0, SW, 65, (Color){20, 50, 30, 255});
    BresenhamLine(0, 65, SW, 65, (Color){80, 160, 60, 200});
    DrawText("TENTANG SIMULASI", (SW - MeasureText("TENTANG SIMULASI", 26)) / 2,
             18, 26, (Color){200, 240, 160, 255});

    // Dekorasi lingkaran — Midcircle dari repo
    Midcircle(SW/2, 160, 60, (Color){50, 120, 50, 180});
    Midcircle(SW/2, 160, 45, (Color){34, 85,  34, 220});
    // Ikon pohon sederhana di tengah lingkaran — BresenhamLine repo
    BresenhamLine(SW/2, 140, SW/2 - 25, 175, (Color){100,200,80,255});
    BresenhamLine(SW/2, 140, SW/2 + 25, 175, (Color){100,200,80,255});
    BresenhamLine(SW/2 - 25, 175, SW/2 + 25, 175, (Color){100,200,80,255});
    BresenhamLine(SW/2, 175, SW/2, 190, (Color){120,80,30,255});

    int cx = SW / 2;
    int y  = 235;

    // Nama simulasi
    const char *simName = "Simulasi Penyebaran Kebakaran Hutan 2D";
    DrawText(simName, cx - MeasureText(simName, 18)/2, y, 18,
             (Color){200, 240, 160, 255});
    y += 30;

    // Garis separator
    BresenhamLine(cx - 200, y, cx + 200, y, (Color){80,160,60,150});
    y += 20;

    // Deskripsi
    const char *desc[] = {
        "Simulasi ini memodelkan fenomena penyebaran api di hutan",
        "menggunakan pendekatan Cellular Automaton pada grid 2D.",
        "",
        "Setiap pohon direpresentasikan sebagai sel dengan state:",
        "Sehat, Terbakar, dan Hangus.",
        "",
        "Probabilitas penyebaran api dipengaruhi oleh:",
        "arah angin, kecepatan angin, dan keberadaan sungai",
        "sebagai penghalang alami (firebreak).",
    };
    for (int i = 0; i < 9; i++) {
        int tw = MeasureText(desc[i], 13);
        DrawText(desc[i], cx - tw/2, y + i*22, 13,
                 (Color){180, 210, 180, 220});
    }
    y += 9*22 + 20;

    // Garis separator
    BresenhamLine(cx - 200, y, cx + 200, y, (Color){80,160,60,150});
    y += 20;

    // Info teknis
    DrawText("Teknologi:", cx - MeasureText("Teknologi:", 14)/2, y, 14,
             (Color){150,220,100,255});
    y += 25;
    const char *tech[] = {
        "Bahasa  : C",
        "Library : Raylib (hanya DrawPixel & DrawText)",
        "Algoritma : DDA, Bresenham, Midpoint Circle",
        "Mata Kuliah : Grafika Komputer 2D",
    };
    for (int i = 0; i < 4; i++) {
        int tw = MeasureText(tech[i], 13);
        DrawText(tech[i], cx - tw/2, y + i*22, 13, WHITE);
    }

    // Tombol kembali
    if (DrawButton(SW/2 - 100, SH - 60, 200, 40,
                   (Color){40, 80, 40, 255}, (Color){100, 200, 80, 255},
                   "< Back", WHITE)) {
        EndDrawing();
        return SCREEN_MENU;
    }

    EndDrawing();
    return SCREEN_ABOUT;
}
