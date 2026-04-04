#include "fire_simulation.h"
#include "terrain.h"
#include "grid.h"
#include "tree.h"
#include "wind.h"
#include "src/ui/ui.h"
#include "src/screens/simulasi/menu.h"
#include "src/algo/bresenham.h"
#include "raylib.h"
#include <stdio.h>

static ForestGrid forest;
static int   igniteMode  = 0;
static int   rainActive  = 0;
static float rainTimer   = 0.0f;

#define RAIN_COUNT 120
static float rainX[RAIN_COUNT];
static float rainY[RAIN_COUNT];
static float rainSpeedY[RAIN_COUNT];

#define COMPASS_CX  900
#define COMPASS_CY  120

#define UI_X        820
#define UI_W        170
#define UI_PAD       10
#define BTN_W       150
#define BTN_H        30
#define BTN_IGNITE_Y  530
#define BTN_RAIN_Y    570
#define BTN_RESET_Y   615
#define SLIDER_X    400
#define SLIDER_Y    600
#define SLIDER_W    200

// static void DrawButton(int x, int y, int w, int h,
//                         Color fill, Color border,
//                         const char *label, Color textCol) {
//     for (int row = y; row <= y + h; row++)
//         BresenhamLine(x, row, x + w, row, fill);
//     BresenhamLine(x,     y,     x + w, y,     border);
//     BresenhamLine(x,     y + h, x + w, y + h, border);
//     BresenhamLine(x,     y,     x,     y + h, border);
//     BresenhamLine(x + w, y,     x + w, y + h, border);
//     int tw = MeasureText(label, 14);
//     DrawText(label, x + (w - tw) / 2, y + (h - 14) / 2, 14, textCol);
// }

// static int IsMouseOver(int x, int y, int w, int h) {
//     Vector2 m = GetMousePosition();
//     return (m.x >= x && m.x <= x + w && m.y >= y && m.y <= y + h);
// }

// static void DrawSlider(int x, int y, int w, float value,
//                         Color track, Color thumb) {
//     for (int off = -2; off <= 2; off++)
//         BresenhamLine(x, y + off, x + w, y + off, track);
//     int tx = x + (int)(value * w);
//     for (int row = y - 7; row <= y + 7; row++)
//         BresenhamLine(tx - 5, row, tx + 5, row, thumb);
//     BresenhamLine(tx - 5, y - 7, tx + 5, y - 7, WHITE);
//     BresenhamLine(tx - 5, y + 7, tx + 5, y + 7, WHITE);
//     BresenhamLine(tx - 5, y - 7, tx - 5, y + 7, WHITE);
//     BresenhamLine(tx + 5, y - 7, tx + 5, y + 7, WHITE);
// }

static void InitRain(void) {
    for (int i = 0; i < RAIN_COUNT; i++) {
        rainX[i]      = (float)GetRandomValue(0, SCREEN_W);
        rainY[i]      = (float)GetRandomValue(-200, 0);
        rainSpeedY[i] = 200.0f + GetRandomValue(0, 100);
    }
}

static void UpdateDrawRain(float dt) {
    Color rainCol = (Color){150, 200, 255, 180};
    for (int i = 0; i < RAIN_COUNT; i++) {
        rainY[i] += rainSpeedY[i] * dt;
        if (rainY[i] > SCREEN_H) {
            rainY[i] = (float)GetRandomValue(-100, 0);
            rainX[i] = (float)GetRandomValue(0, SCREEN_W);
        }
        int x1 = (int)rainX[i];
        int y1 = (int)rainY[i];
        BresenhamLine(x1, y1, x1 - 2, y1 + 8, rainCol);
    }
}

static void CountTrees(int *h, int *b, int *d) {
    *h = *b = *d = 0;
    for (int r = 0; r < GRID_ROWS; r++)
        for (int c = 0; c < GRID_COLS; c++) {
            TreeState s = forest.trees[r][c].state;
            if      (s == TREE_HEALTHY) (*h)++;
            else if (s == TREE_BURNING) (*b)++;
            else                        (*d)++;
        }
}

void FireSimInit(void) {
    GridInit(&forest, 20, 220, 64, 32);
    WindInit();
    InitRain();
}

ScreenType FireSimUpdate(void) {
    float dt = GetFrameTime();

    // Di bagian DRAW, ubah pemanggilan DrawSlider jadi:
    float newSpd = DrawSlider(SLIDER_X, SLIDER_Y, SLIDER_W,
                            WindGetSpeed(),
                            (Color){70,70,90,255}, (Color){255,180,50,255}, NULL);
    WindSetSpeed(newSpd);

    // Tombol Picu Api
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) &&
        IsMouseOver(UI_X + UI_PAD, BTN_IGNITE_Y, BTN_W, BTN_H))
        igniteMode = !igniteMode;

    // Klik pohon saat ignite mode
    if (igniteMode && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        Vector2 m = GetMousePosition();
        if (m.x < UI_X) {
            int col, row;
            if (GridPickCell(&forest, (int)m.x, (int)m.y, &col, &row))
                GridIgnite(&forest, col, row);
        }
    }

    // Tombol Hujan
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) &&
        IsMouseOver(UI_X + UI_PAD, BTN_RAIN_Y, BTN_W, BTN_H)) {
        rainActive = 1;
        rainTimer  = 5.0f;
        InitRain();
    }
    if (rainActive) {
        rainTimer -= dt;
        if (rainTimer <= 0.0f) rainActive = 0;
        for (int r = 0; r < GRID_ROWS; r++)
            for (int c = 0; c < GRID_COLS; c++)
                if (forest.trees[r][c].state == TREE_BURNING)
                    forest.trees[r][c].burnTimer -= dt * 2.0f;
    }

    // Tombol Reset
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) &&
        IsMouseOver(UI_X + UI_PAD, BTN_RESET_Y, BTN_W, BTN_H)) {
        GridReset(&forest);
        rainActive = 0;
        igniteMode = 0;
    }

    // Update grid & angin
    GridUpdate(&forest, dt, WindGetDirX(), WindGetDirY(), WindGetSpeed());
    WindUpdate(dt, COMPASS_CX, COMPASS_CY);

    // ================================================================
    // DRAW
    // ================================================================
    BeginDrawing();
    ClearBackground((Color){135, 190, 230, 255});

    TerrainDraw();
    WindDraw(COMPASS_CX, COMPASS_CY);   // garis angin + kompas
    GridDraw(&forest);
    if (rainActive) UpdateDrawRain(dt);

    // Panel UI
    for (int row = 490; row <= SCREEN_H - 5; row++)
        BresenhamLine(UI_X, row, UI_X + UI_W, row, (Color){25, 25, 35, 210});
    BresenhamLine(UI_X, 490, UI_X + UI_W, 490, (Color){80,80,100,255});
    BresenhamLine(UI_X, 490, UI_X, SCREEN_H - 5, (Color){80,80,100,255});

    // Slider kecepatan angin
    DrawText("Kecepatan Angin:", 400 + UI_PAD, SLIDER_Y - 22, 11, LIGHTGRAY);
    DrawSlider(SLIDER_X, SLIDER_Y, SLIDER_W,
               WindGetSpeed(),
               (Color){70,70,90,255}, (Color){255,180,50,255}, NULL);
    char spdLabel[8];
    sprintf(spdLabel, "%.0f%%", WindGetSpeed() * 100);
    DrawText(spdLabel, SLIDER_X + SLIDER_W + 4, SLIDER_Y - 6, 11, WHITE);

    // Tombol
    DrawButton(UI_X+UI_PAD, BTN_IGNITE_Y, BTN_W, BTN_H,
               igniteMode ? (Color){200,80,30,255} : (Color){140,50,15,255},
               (Color){255,120,50,255},
               igniteMode ? "[PICU API: ON]" : "Picu Api", WHITE);

    DrawButton(UI_X+UI_PAD, BTN_RAIN_Y, BTN_W, BTN_H,
               rainActive ? (Color){30,80,180,255} : (Color){20,55,130,255},
               (Color){80,150,255,255},
               rainActive ? "[HUJAN...]" : "Hujan", WHITE);

    DrawButton(UI_X+UI_PAD, BTN_RESET_Y, BTN_W, BTN_H,
               (Color){45,45,55,255}, (Color){130,130,150,255},
               "Reset", WHITE);

    // Counter pohon
    int healthy, burning, burned;
    CountTrees(&healthy, &burning, &burned);
    char buf[32];
    DrawText("-- Status Hutan --", 10, 560, 11, LIGHTGRAY);
    sprintf(buf, "Sehat   : %d", healthy);  DrawText(buf, 10, 580, 12, GREEN);
    sprintf(buf, "Terbakar: %d", burning);  DrawText(buf, 10, 600, 12, ORANGE);
    sprintf(buf, "Hangus  : %d", burned);   DrawText(buf, 10, 620, 12, GRAY);

    // DrawText("Klik pohon untuk mulai api (aktifkan Picu Api dulu) | Drag kompas = ubah arah angin",
    //          10, SCREEN_H - 18, 10, (Color){180,180,180,200});
    
    if (DrawButton(10, 10, 120, 30,
                (Color){30,30,30,200}, (Color){80,160,60,255},
                "< Back", WHITE)) {
        EndDrawing();
        return SCREEN_MENU;
    }
    
    if (IsKeyPressed(KEY_ESCAPE)) {
        EndDrawing();
        return SCREEN_MENU;
    }

    EndDrawing();
    return SCREEN_SIMULASI;
}

void FireSimUnload(void) {}
