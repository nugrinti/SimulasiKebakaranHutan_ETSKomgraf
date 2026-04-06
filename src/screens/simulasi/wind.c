#include "wind.h"
#include "src/algo/dda.h"          
#include "src/algo/bresenham.h"     
#include "src/algo/midcircle.h"     
#include "raylib.h"
#include <math.h>

// =============================================================================
// State internal angin
// =============================================================================
static float windAngle = 0.0f;    // derajat: 0=kanan, 90=bawah, 180=kiri, 270=atas
static float windSpd   = 0.5f;    // 0..1

// =============================================================================
// Partikel garis angin — bergerak di area hutan
// =============================================================================
#define WIND_LINE_COUNT  30       // jumlah garis angin di layar

typedef struct {
    float x, y;       // posisi kepala garis
    float alpha;      // transparansi 0..1 (fade in/out)
    float life;       // 0..1, posisi dalam siklus hidup
} WindLine;

static WindLine wlines[WIND_LINE_COUNT];

// Area kemunculan garis angin (area hutan di layar)
#define WIND_AREA_X1   20
#define WIND_AREA_X2  800
#define WIND_AREA_Y1  220
#define WIND_AREA_Y2  480

// =============================================================================
// WindInit
// =============================================================================
void WindInit(void) {
    for (int i = 0; i < WIND_LINE_COUNT; i++) {
        wlines[i].x     = (float)GetRandomValue(WIND_AREA_X1, WIND_AREA_X2);
        wlines[i].y     = (float)GetRandomValue(WIND_AREA_Y1, WIND_AREA_Y2);
        wlines[i].life  = (float)GetRandomValue(0, 100) / 100.0f; // acak fase awal
        wlines[i].alpha = 0.0f;
    }
}

// =============================================================================
// WindUpdate
//   - Gerakkan tiap garis searah angin
//   - Fade in saat lahir, fade out saat mati
//   - Reset ke posisi baru saat keluar layar
//   - Baca input mouse untuk drag kompas
// =============================================================================
void WindUpdate(float dt, int compassCX, int compassCY) {

    // ------------------------------------------------------------------
    // Update posisi garis angin
    // ------------------------------------------------------------------
    float rad   = windAngle * DEG2RAD;
    float dirX  = cosf(rad);
    float dirY  = sinf(rad);
    float speed = windSpd * 120.0f + 20.0f;  // piksel/detik, min 20

    for (int i = 0; i < WIND_LINE_COUNT; i++) {
        wlines[i].life += dt * 0.6f;

        // Fade in 0..0.2, full 0.2..0.8, fade out 0.8..1.0
        float l = wlines[i].life;
        if      (l < 0.2f) wlines[i].alpha = l / 0.2f;
        else if (l < 0.8f) wlines[i].alpha = 1.0f;
        else if (l < 1.0f) wlines[i].alpha = (1.0f - l) / 0.2f;
        else {
            // Reset partikel ke posisi baru
            // Spawn dari sisi berlawanan arah angin agar terlihat mengalir
            wlines[i].life  = 0.0f;
            wlines[i].alpha = 0.0f;
            // Posisi spawn acak di tepi area hutan sesuai arah angin
            if (fabsf(dirX) > fabsf(dirY)) {
                // Angin dominan horizontal → spawn dari kiri atau kanan
                wlines[i].x = (dirX > 0) ? (float)WIND_AREA_X1
                                          : (float)WIND_AREA_X2;
                wlines[i].y = (float)GetRandomValue(WIND_AREA_Y1, WIND_AREA_Y2);
            } else {
                // Angin dominan vertikal → spawn dari atas atau bawah
                wlines[i].x = (float)GetRandomValue(WIND_AREA_X1, WIND_AREA_X2);
                wlines[i].y = (dirY > 0) ? (float)WIND_AREA_Y1
                                          : (float)WIND_AREA_Y2;
            }
            continue;
        }

        // Gerakkan garis searah angin
        wlines[i].x += dirX * speed * dt;
        wlines[i].y += dirY * speed * dt;

        // Jika keluar area, paksa reset
        if (wlines[i].x < WIND_AREA_X1 - 20 || wlines[i].x > WIND_AREA_X2 + 20 ||
            wlines[i].y < WIND_AREA_Y1 - 20 || wlines[i].y > WIND_AREA_Y2 + 20) {
            wlines[i].life = 1.1f;  // paksa reset di iterasi berikutnya
        }
    }

    // ------------------------------------------------------------------
    // Input kompas — drag mouse untuk ubah arah angin
    // Radius interaktif kompas = 40px dari pusat
    // ------------------------------------------------------------------
    #define COMPASS_R 40

    Vector2 mouse = GetMousePosition();
    float   dx    = mouse.x - (float)compassCX;
    float   dy    = mouse.y - (float)compassCY;
    float   dist  = sqrtf(dx*dx + dy*dy);

    if (IsMouseButtonDown(MOUSE_LEFT_BUTTON) && dist <= (float)COMPASS_R) {
        // atan2f mengembalikan radian, konversi ke derajat
        windAngle = atan2f(dy, dx) * RAD2DEG;
        if (windAngle < 0.0f) windAngle += 360.0f;
    }
}

// =============================================================================
// WindDraw
// =============================================================================
void WindDraw(int compassCX, int compassCY) {

    float rad  = windAngle * DEG2RAD;
    float dirX = cosf(rad);
    float dirY = sinf(rad);

    // ------------------------------------------------------------------
    // 1. Garis-garis angin bergerak
    // ------------------------------------------------------------------
    int lineLen = (int)(windSpd * 18.0f) + 6;   // 6..24 px

    for (int i = 0; i < WIND_LINE_COUNT; i++) {
        if (wlines[i].alpha < 0.05f) continue;

        unsigned char a = (unsigned char)(wlines[i].alpha * 140 * windSpd + 10);
        Color wCol = (Color){220, 230, 255, a};

        int x1 = (int)wlines[i].x;
        int y1 = (int)wlines[i].y;
        int x2 = x1 + (int)(dirX * lineLen);
        int y2 = y1 + (int)(dirY * lineLen);

        DDALine(x1, y1, x2, y2, wCol);

        // Kepala panah kecil (2 garis pendek)
        // Sudut panah ±30 derajat dari arah berlawanan
        float backRad = rad + (float)M_PI;
        float arrowLen = (float)lineLen * 0.35f;
        int ax1 = x2 + (int)(cosf(backRad + 0.5f) * arrowLen);
        int ay1 = y2 + (int)(sinf(backRad + 0.5f) * arrowLen);
        int ax2 = x2 + (int)(cosf(backRad - 0.5f) * arrowLen);
        int ay2 = y2 + (int)(sinf(backRad - 0.5f) * arrowLen);
        DDALine(x2, y2, ax1, ay1, wCol);
        DDALine(x2, y2, ax2, ay2, wCol);
    }

    // ------------------------------------------------------------------
    // 2. Kompas interaktif
    // ------------------------------------------------------------------
    #define COMPASS_R     40
    #define COMPASS_R2    28   // radius jarum

    // Lingkaran luar kompas
    Midcircle(compassCX, compassCY, COMPASS_R,
              (Color){180, 180, 200, 220});

    // Lingkaran dalam (dekoratif)
    Midcircle(compassCX, compassCY, COMPASS_R - 4,
              (Color){100, 100, 130, 150});

    // Background kompas 
    for (int dy2 = -COMPASS_R + 1; dy2 < COMPASS_R; dy2++) {
        float hw = sqrtf((float)(COMPASS_R * COMPASS_R - dy2 * dy2));
        int x1   = compassCX - (int)hw + 1;
        int x2   = compassCX + (int)hw - 1;
        unsigned char alpha = 160;
        BresenhamLine(x1, compassCY + dy2, x2, compassCY + dy2,
                      (Color){20, 25, 40, alpha});
    }
    
    Midcircle(compassCX, compassCY, COMPASS_R,
              (Color){200, 200, 230, 255});

    // Tanda arah mata angin (N, S, E, W) 
    DrawText("U", compassCX - 4,  compassCY - COMPASS_R - 14, 11, WHITE);
    DrawText("S", compassCX - 4,  compassCY + COMPASS_R + 3,  11, WHITE);
    DrawText("T", compassCX + COMPASS_R + 3,  compassCY - 5,  11, WHITE);
    DrawText("B", compassCX - COMPASS_R - 12, compassCY - 5,  11, WHITE);

    // Jarum kompas 
    // Ujung jarum mengikuti windAngle
    int needleX = compassCX + (int)(dirX * COMPASS_R2);
    int needleY = compassCY + (int)(dirY * COMPASS_R2);

    // Jarum utama (oranye tebal = 3 garis paralel)
    BresenhamLine(compassCX, compassCY, needleX, needleY,
                  (Color){255, 150, 30, 255});
    BresenhamLine(compassCX + 1, compassCY, needleX + 1, needleY,
                  (Color){255, 150, 30, 200});
    BresenhamLine(compassCX, compassCY + 1, needleX, needleY + 1,
                  (Color){255, 150, 30, 200});

    // Ekor jarum (biru, arah berlawanan, lebih pendek)
    int tailX = compassCX - (int)(dirX * 15);
    int tailY = compassCY - (int)(dirY * 15);
    BresenhamLine(compassCX, compassCY, tailX, tailY,
                  (Color){80, 130, 255, 255});

    // Titik tengah kompas
    DrawPixel(compassCX,     compassCY,     WHITE);
    DrawPixel(compassCX + 1, compassCY,     WHITE);
    DrawPixel(compassCX,     compassCY + 1, WHITE);
    DrawPixel(compassCX + 1, compassCY + 1, WHITE);

    // Label sudut angin
    // char angLabel[20];
    int  degInt = (int)windAngle;
    const char *dirName;
    if      (degInt < 23  || degInt >= 338) dirName = "Timur";
    else if (degInt < 68)                   dirName = "Tenggara";
    else if (degInt < 113)                  dirName = "Selatan";
    else if (degInt < 158)                  dirName = "Barat Daya";
    else if (degInt < 203)                  dirName = "Barat";
    else if (degInt < 248)                  dirName = "Barat Laut";
    else if (degInt < 293)                  dirName = "Utara";
    else                                    dirName = "Timur Laut";

    DrawText("Arah Angin:", compassCX - COMPASS_R, compassCY + COMPASS_R + 18, 11, LIGHTGRAY);
    DrawText(dirName,       compassCX - COMPASS_R, compassCY + COMPASS_R + 30, 11, (Color){255, 180, 50, 255});
}

// =============================================================================
// Getter & Setter
// =============================================================================
float WindGetDirX(void)       { return cosf(windAngle * DEG2RAD); }
float WindGetDirY(void)       { return sinf(windAngle * DEG2RAD); }
float WindGetSpeed(void)      { return windSpd; }
void  WindSetSpeed(float s)   { windSpd = s; if (windSpd < 0) windSpd = 0; if (windSpd > 1) windSpd = 1; }
