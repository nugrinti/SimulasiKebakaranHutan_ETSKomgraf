#include "help.h"
#include "src/ui/ui.h"
#include "src/screens/simulasi/menu.h"
#include "../../../src/algo/bresenham.h"
#include "raylib.h"

#define SW 1000
#define SH  700

ScreenType HelpUpdate(void) {
    BeginDrawing();
    ClearBackground((Color){15, 25, 35, 255});

    // Header
    FillRectUI(0, 0, SW, 65, (Color){20, 50, 30, 255});
    BresenhamLine(0, 65, SW, 65, (Color){80, 160, 60, 200});
    DrawText("CARA PENGGUNAAN", (SW - MeasureText("CARA PENGGUNAAN", 26)) / 2,
             18, 26, (Color){200, 240, 160, 255});

    // Konten
    int x = 80, y = 100, gap = 32;

    DrawText("LANGKAH MENJALANKAN SIMULASI:", x, y, 16,
             (Color){150, 220, 100, 255});
    BresenhamLine(x, y + 20, x + 380, y + 20, (Color){80, 160, 60, 150});

    y += 35;
    const char *steps[] = {
        "1. Tekan tombol  [ Picu Api ]  untuk mengaktifkan mode ignite",
        "2. Klik pada pohon di area hutan untuk memulai kebakaran",
        "3. Api akan menyebar ke pohon tetangga secara otomatis",
        "4. Drag kompas di pojok kanan atas untuk mengubah arah angin",
        "5. Geser slider untuk mengatur kecepatan angin (0% - 100%)",
        "6. Tekan tombol  [ Hujan ]  untuk memadamkan api selama 5 detik",
        "7. Tekan tombol  [ Reset ]  untuk memulai ulang seluruh simulasi",
    };
    for (int i = 0; i < 7; i++) {
        DrawText(steps[i], x, y + i * gap, 14, WHITE);
    }

    y += 7 * gap + 20;
    DrawText("PENGARUH ANGIN:", x, y, 16, (Color){150, 220, 100, 255});
    BresenhamLine(x, y + 20, x + 260, y + 20, (Color){80, 160, 60, 150});
    y += 35;
    const char *windInfo[] = {
        "- Garis putih bergerak menunjukkan arah & kecepatan angin",
        "- Api menyebar lebih cepat searah angin",
        "- Sungai berfungsi sebagai firebreak - api tidak bisa menyeberang",
    };
    for (int i = 0; i < 3; i++) {
        DrawText(windInfo[i], x, y + i * gap, 14, (Color){200, 220, 255, 220});
    }

    y += 3 * gap + 20;
    DrawText("STATUS POHON:", x, y, 16, (Color){150, 220, 100, 255});
    BresenhamLine(x, y + 20, x + 220, y + 20, (Color){80, 160, 60, 150});
    y += 35;

    // Indikator warna status pohon
    FillRectUI(x, y, x + 20, y + 16, GREEN);
    DrawText("Sehat   - pohon normal, bisa terbakar", x + 28, y, 14, WHITE);
    y += gap;
    FillRectUI(x, y, x + 20, y + 16, ORANGE);
    DrawText("Terbakar - sedang terbakar, menyebar ke tetangga", x + 28, y, 14, WHITE);
    y += gap;
    FillRectUI(x, y, x + 20, y + 16, GRAY);
    DrawText("Hangus  - sudah padam, tidak bisa terbakar lagi", x + 28, y, 14, WHITE);

    // Tombol kembali
    if (DrawButton(SW/2 - 100, SH - 60, 200, 40,
                   (Color){40, 80, 40, 255}, (Color){100, 200, 80, 255},
                   "< Back", WHITE)) {
        EndDrawing();
        return SCREEN_MENU;
    }

    EndDrawing();
    return SCREEN_HELP;
}
