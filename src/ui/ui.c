#include "ui.h"
#include "src/algo/bresenham.h"  
#include "raylib.h"

// =============================================================================
// FillRectUI — isi persegi panjang dengan scan-fill BresenhamLine
// =============================================================================
void FillRectUI(int x1, int y1, int x2, int y2, Color col) {
    for (int y = y1; y <= y2; y++)
        BresenhamLine(x1, y, x2, y, col);
}

// =============================================================================
// IsMouseOver
// =============================================================================
int IsMouseOver(int x, int y, int w, int h) {
    Vector2 m = GetMousePosition();
    return (m.x >= x && m.x <= x + w && m.y >= y && m.y <= y + h);
}

// =============================================================================
// DrawButton
// Mengembalikan 1 jika tombol diklik
// =============================================================================
int DrawButton(int x, int y, int w, int h,
               Color fill, Color border,
               const char *label, Color textCol)
{
    int hovered = IsMouseOver(x, y, w, h);

    // Sedikit terang saat hover
    Color actualFill = fill;
    if (hovered) {
        actualFill.r = (unsigned char)(fill.r + 20 > 255 ? 255 : fill.r + 20);
        actualFill.g = (unsigned char)(fill.g + 20 > 255 ? 255 : fill.g + 20);
        actualFill.b = (unsigned char)(fill.b + 20 > 255 ? 255 : fill.b + 20);
    }

    // Isi tombol
    FillRectUI(x, y, x + w, y + h, actualFill);

    // Border 4 sisi
    BresenhamLine(x,     y,     x + w, y,     border);
    BresenhamLine(x,     y + h, x + w, y + h, border);
    BresenhamLine(x,     y,     x,     y + h, border);
    BresenhamLine(x + w, y,     x + w, y + h, border);

    // Label teks di tengah
    int tw = MeasureText(label, 16);
    DrawText(label, x + (w - tw) / 2, y + (h - 16) / 2, 16, textCol);

    return (hovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON));
}

// =============================================================================
// DrawSlider — slider horizontal, kembalikan nilai baru
// =============================================================================
float DrawSlider(int x, int y, int w, float value,
                 Color track, Color thumb, const char *label)
{
    // Track
    for (int off = -3; off <= 3; off++)
        BresenhamLine(x, y + off, x + w, y + off, track);

    // Drag input
    if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
        Vector2 m = GetMousePosition();
        if (m.x >= x && m.x <= x + w &&
            m.y >= y - 15 && m.y <= y + 15) {
            value = (m.x - x) / (float)w;
            if (value < 0.0f) value = 0.0f;
            if (value > 1.0f) value = 1.0f;
        }
    }

    // Thumb
    int tx = x + (int)(value * w);
    FillRectUI(tx - 6, y - 8, tx + 6, y + 8, thumb);
    BresenhamLine(tx - 6, y - 8, tx + 6, y - 8, WHITE);
    BresenhamLine(tx - 6, y + 8, tx + 6, y + 8, WHITE);
    BresenhamLine(tx - 6, y - 8, tx - 6, y + 8, WHITE);
    BresenhamLine(tx + 6, y - 8, tx + 6, y + 8, WHITE);

    // Label + nilai
    if (label) {
        DrawText(label, x, y - 22, 12, LIGHTGRAY);
        // char val[8];
        // sprintf tidak tersedia tanpa stdio — pakai TextFormat raylib
        DrawText(TextFormat("%.0f%%", value * 100), x + w + 6, y - 6, 12, WHITE);
    }

    return value;
}

// =============================================================================
// DrawTextBlock — gambar teks multiline dengan \n sebagai pemisah
// =============================================================================
void DrawTextBlock(const char *text, int x, int y, int fontSize, Color col) {
    char buf[512];
    int  len   = 0;
    int  lineY = y;
    int  lineH = fontSize + 4;

    for (int i = 0; text[i] != '\0'; i++) {
        if (text[i] == '\n' || text[i+1] == '\0') {
            if (text[i] != '\n') buf[len++] = text[i];
            buf[len] = '\0';
            DrawText(buf, x, lineY, fontSize, col);
            lineY += lineH;
            len = 0;
        } else {
            buf[len++] = text[i];
        }
    }
}
