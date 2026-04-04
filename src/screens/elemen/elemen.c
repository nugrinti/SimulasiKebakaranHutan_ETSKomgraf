#include "elemen.h"
#include "src/ui/ui.h"
#include "src/screens/simulasi/tree.h"
#include "../../../src/algo/bresenham.h"
#include "../../../src/algo/midcircle.h"
#include "../../../src/algo/dda.h"
#include "raylib.h"
#include <math.h>

#define SW 1000
#define SH  700

// =============================================================================
// Halaman elemen — ditampilkan per halaman (page 0..3)
// =============================================================================
static int page = 0;
#define PAGE_COUNT 4

// =============================================================================
// Helper: gambar kotak info elemen
// judul, deskripsi, dan gambar elemen di sisi kiri
// =============================================================================
static void DrawElemenCard(int x, int y, int w, int h,
                            const char *judul, const char *koord,
                            const char *desc[], int descCount,
                            Color accent)
{
    // Background kartu
    FillRectUI(x, y, x + w, y + h, (Color){20, 35, 25, 240});
    // Border kartu
    BresenhamLine(x,     y,     x + w, y,     accent);
    BresenhamLine(x,     y + h, x + w, y + h, accent);
    BresenhamLine(x,     y,     x,     y + h, accent);
    BresenhamLine(x + w, y,     x + w, y + h, accent);

    // Judul
    DrawText(judul, x + 10, y + 10, 16, accent);
    BresenhamLine(x + 10, y + 30, x + w - 10, y + 30,
                  (Color){accent.r, accent.g, accent.b, 100});

    // Koordinat utama
    DrawText(koord, x + 10, y + 38, 11, (Color){180, 220, 150, 200});

    // Deskripsi
    for (int i = 0; i < descCount; i++)
        DrawText(desc[i], x + 10, y + 58 + i * 19, 12, (Color){200,210,200,220});
}

// =============================================================================
// Gambar pohon sehat mini untuk kartu elemen
// Reuse TreeDraw() — tapi buat pohon mini langsung di sini
// menggunakan BresenhamLine + FillTriangle manual
// =============================================================================
static void FillTri(int tx, int ty, int blx, int bly, int brx, int bry, Color c) {
    for (int y = ty; y <= bly; y++) {
        float t = (bly==ty) ? 1.0f : (float)(y-ty)/(float)(bly-ty);
        int xl = (int)(tx + t*(blx-tx));
        int xr = (int)(tx + t*(brx-tx));
        BresenhamLine(xl, y, xr, y, c);
    }
}

static void DrawMiniTree(int cx, int by, int state) {
    // batang
    Color trunkCol = state==2 ? (Color){30,30,30,255} : (Color){101,67,33,255};
    for (int x = cx-4; x <= cx+4; x++)
        BresenhamLine(x, by-22, x, by, trunkCol);

    if (state == 0) { // sehat
        FillTri(cx, by-46, cx-18, by-22, cx+18, by-22, (Color){34,85,34,255});
        FillTri(cx, by-58, cx-13, by-40, cx+13, by-40, (Color){50,120,50,255});
        FillTri(cx, by-68, cx-8,  by-54, cx+8,  by-54, (Color){80,160,60,255});
    } else if (state == 1) { // terbakar
        FillTri(cx, by-46, cx-18, by-22, cx+18, by-22, (Color){220,100,20,255});
        FillTri(cx, by-58, cx-13, by-40, cx+13, by-40, (Color){180,40,10,255});
        BresenhamLine(cx, by-58, cx, by-70, (Color){255,200,50,255});
    } else { // hangus
        FillTri(cx, by-36, cx-10, by-22, cx+10, by-22, (Color){80,80,80,200});
    }
}

// =============================================================================
// ElemenUpdate — gambar halaman elemen sesuai page aktif
// =============================================================================
ScreenType ElemenUpdate(void) {
    BeginDrawing();
    ClearBackground((Color){12, 20, 15, 255});

    // Header
    FillRectUI(0, 0, SW, 65, (Color){20, 50, 30, 255});
    BresenhamLine(0, 65, SW, 65, (Color){80, 160, 60, 200});
    DrawText("ELEMEN SIMULASI",
             (SW - MeasureText("ELEMEN SIMULASI", 26)) / 2,
             18, 26, (Color){200, 240, 160, 255});

    // Indikator halaman
    DrawText(TextFormat("Halaman %d / %d", page+1, PAGE_COUNT),
             SW - 160, 20, 13, (Color){150, 200, 120, 200});

    // ----------------------------------------------------------------
    // HALAMAN 0 — Pohon (3 state)
    // ----------------------------------------------------------------
    if (page == 0) {
        DrawText("OBJEK: POHON", 60, 85, 18, (Color){150,220,100,255});

        // Pohon sehat
        DrawElemenCard(40, 115, 280, 480,
            "Pohon Sehat",
            "Mahkota: segitiga 3 lapis",
            (const char*[]){
                "Dibangun dari 3 segitiga",
                "berlapis dengan scan-fill",
                "manual (BresenhamLine).",
                "",
                "Titik mahkota layer 1:",
                "  puncak : (cx, ty-cs)",
                "  kiri   : (cx-cs, ty)",
                "  kanan  : (cx+cs, ty)",
                "",
                "Batang: FillRect manual",
                "  kiri  : cx - trunkW/2",
                "  kanan : cx + trunkW/2",
                "",
                "Bayangan: ellips manual",
                "  rx = crownSize",
                "  ry = crownSize / 4",
            }, 16,
            (Color){80,200,80,255});
        DrawMiniTree(180, 560, 0);

        // Pohon terbakar
        DrawElemenCard(340, 115, 280, 480,
            "Pohon Terbakar",
            "Mahkota: oranye + lidah api",
            (const char*[]){
                "State BURNING aktif saat",
                "api menyebar dari tetangga.",
                "",
                "Lidah api = BresenhamLine",
                "pendek ke atas dari puncak:",
                "  (cx, tipY) → (cx, tipY-8)",
                "  (cx±3, tipY-2) → atas",
                "",
                "burnTimer countdown: 3 dtk",
                "Saat timer = 0 → BURNED",
                "",
                "Warna api:",
                "  kuning : (255,200,50)",
                "  oranye : (220,100,20)",
                "  merah  : (180,40,10)",
            }, 15,
            (Color){255,140,30,255});
        DrawMiniTree(480, 560, 1);

        // Pohon hangus
        DrawElemenCard(640, 115, 280, 480,
            "Pohon Hangus",
            "Hanya batang + abu kecil",
            (const char*[]){
                "State BURNED = pohon",
                "tidak bisa terbakar lagi.",
                "Berfungsi sebagai barrier",
                "alami setelah padam.",
                "",
                "Visual: batang hitam +",
                "segitiga abu kecil di atas",
                "  warna : (80,80,80,200)",
                "  cs    : crownSize / 3",
                "",
                "Titik abu beterbangan:",
                "  DrawPixel() langsung",
                "  posisi acak di sekitar",
                "  puncak batang",
            }, 14,
            (Color){150,150,150,255});
        DrawMiniTree(780, 560, 2);
    }

    // ----------------------------------------------------------------
    // HALAMAN 1 — Angin & Sungai
    // ----------------------------------------------------------------
    else if (page == 1) {
        DrawText("OBJEK: ANGIN", 60, 85, 18, (Color){150,220,100,255});

        // Angin
        DrawElemenCard(40, 115, 430, 480,
            "Sistem Angin",
            "DDALine() - arah bebas diagonal",
            (const char*[]){
                "Garis angin divisualisasikan",
                "dengan DDALine() karena",
                "mendukung semua sudut.",
                "",
                "Posisi partikel angin:",
                "  x += cos(angle) * speed * dt",
                "  y += sin(angle) * speed * dt",
                "",
                "Kepala panah arah angin:",
                "  backRad = angle + PI",
                "  ax = x2 + cos(backRad±0.5)*L",
                "  ay = y2 + sin(backRad±0.5)*L",
                "",
                "Kompas interaktif:",
                "  Midcircle() untuk lingkaran",
                "  BresenhamLine() untuk jarum",
                "  angle = atan2(dy,dx) saat drag",
                "",
                "Pengaruh ke penyebaran api:",
                "  windDot = dot(windDir, spread)",
                "  prob += windDot * windSpd * 0.4",
            }, 20,
            (Color){180,200,255,255});

        // Visualisasi garis angin
        int wx = 370, wy = 330+50;
        for (int i = 0; i < 5; i++) {
            int x1 = wx - 60 + i*30, y1 = wy - i*8;
            int x2 = x1 + 25,         y2 = y1 - 10;
            DDALine(x1, y1, x2, y2, (Color){180,200,255,(unsigned char)(100+i*30)});
            DDALine(x2, y2, x2-5, y2+5, (Color){180,200,255,150});
            DDALine(x2, y2, x2+3, y2+6, (Color){180,200,255,150});
        }
        DrawText("visualisasi garis angin", wx - 60, wy + 20, 11,
                 (Color){150,170,220,180});

    //     // Sungai
    //     DrawElemenCard(490, 115, 470, 480,
    //         "Sungai (Firebreak)",
    //         "BresenhamLine() berlapis",
    //         (const char*[]){
    //             "Sungai dibentuk dari array",
    //             "titik (x,y) yang disambung",
    //             "dengan BresenhamLine().",
    //             "",
    //             "Array titik sungai (17 titik):",
    //             "  riverX[] = {120,160,210,...}",
    //             "  riverY[] = {390,410,395,...}",
    //             "",
    //             "Digambar berlapis (lebar 6px):",
    //             "  for off = -3 to +3:",
    //             "    BresenhamLine(x1,y1+off,",
    //             "                 x2,y2+off)",
    //             "",
    //             "Fungsi firebreak:",
    //             "  TerrainIsRiver(x,y) cek",
    //             "  jarak titik ke segmen sungai",
    //             "  Jika < threshold → block api",
    //             "",
    //             "Warna berlapis:",
    //             "  tepi  : (50,100,180)",
    //             "  tengah: (120,180,240)",
    //         }, 20,
    //         (Color){80,150,255,255});
    }

    // ----------------------------------------------------------------
    // HALAMAN 2 — Terrain (Bukit & Tanah)
    // ----------------------------------------------------------------
    else if (page == 2) {
        DrawText("OBJEK: TERRAIN", 60, 85, 18, (Color){150,220,100,255});

        DrawElemenCard(40, 115, 430, 480,
            "Bukit Berlapis",
            "FillTriangle + BresenhamLine",
            (const char*[]){
                "3 bukit segitiga besar,",
                "digambar dari belakang",
                "ke depan (painter algo).",
                "",
                "Bukit 1 (belakang kiri):",
                "  puncak : (200, 220)",
                "  kiri   : (-40, 480)",
                "  kanan  : (440, 480)",
                "",
                "Bukit 2 (belakang kanan):",
                "  puncak : (780, 210)",
                "  kiri   : (540, 480)",
                "  kanan  : (1040,480)",
                "",
                "Bukit 3 (tengah depan):",
                "  puncak : (500, 200)",
                "  kiri   : (220, 480)",
                "  kanan  : (780, 480)",
                "",
                "Isi : scan-fill per baris",
                "  t = (y-yTop)/(yBot-yTop)",
                "  xL = tx + t*(blx-tx)",
            }, 22,
            (Color){100,180,80,255});

        // Visualisasi mini bukit
        int bx = 280, by = 580;
        FillTri(bx, by-80, bx-70, by, bx+70, by, (Color){50,90,50,200});
        FillTri(bx-30, by-60, bx-90, by, bx+10, by, (Color){65,110,55,180});
        DrawText("bukit berlapis", bx-45, by+5, 11, (Color){120,180,80,180});

        DrawElemenCard(490, 115, 470, 480,
            "Tanah & Rumput",
            "FillRect horizontal berlapis",
            (const char*[]){
                "Tanah dibagi 2 lapisan:",
                "",
                "Lapisan rumput (atas):",
                "  y: GROUND_Y → GROUND_Y+12",
                "  warna gelap : (80,140,50)",
                "  warna terang: (100,160,60)",
                "",
                "Lapisan tanah (bawah):",
                "  y: GROUND_Y+12 → SCREEN_H",
                "  warna: (110,75,35)",
                "",
                "Cara menggambar:",
                "  for y = y1 to y2:",
                "    BresenhamLine(0,y,SW,y,col)",
                "",
                "GROUND_Y = 480 (konstanta)",
                "SCREEN_H  = 700",
                "",
                "Warna berubah saat kebakaran",
                "besar (langit jadi oranye)",
                "→ dikontrol dari fire_sim.c",
            }, 20,
            (Color){150,120,60,255});
            // Visualisasi mini tanah & rumput — ambil logika dari terrain.c
            // Rumput
            for (int y = 600-50; y <= 612-50; y++)
                BresenhamLine(470+20, y, 940+20, y,
                            y < 606-50 ? (Color){80,140,50,255} : (Color){100,160,60,255});
            // Tanah
            for (int y = 613-50; y <= 650-50; y++)
                BresenhamLine(470+20, y, 940+20, y, (Color){110,75,35,255});
            DrawText("rumput", 480+20, 615-50, 10, (Color){150,220,80,180});
            DrawText("tanah",  480+20, 635-50, 10, (Color){180,140,80,180});

    }

    // ----------------------------------------------------------------
    // HALAMAN 3 — Grid Cellular Automaton
    // ----------------------------------------------------------------
    else if (page == 3) {
        DrawText("SISTEM: CELLULAR AUTOMATON", 60, 85, 18,
                 (Color){150,220,100,255});

        DrawElemenCard(40, 115, 440, 480,
            "Grid Penyebaran Api",
            "12 kolom x 8 baris = 96 pohon",
            (const char*[]){
                "Setiap frame, pohon BURNING",
                "mencoba menyalakan 4 tetangga:",
                "atas, bawah, kiri, kanan.",
                "",
                "Probabilitas penyebaran/detik:",
                "  baseProb = 0.40",
                "  windBonus = dot * spd * 0.40",
                "  prob = base + windBonus",
                "  min prob = 0.05 (selalu ada)",
                "",
                "Konversi prob/detik /frame:",
                "  frameProb = 1-(1-prob)^dt",
                "",
                "burnTimer tiap pohon acak:",
                "  2.5 + rand(0..3) detik",
                "  saat = 0 -> state BURNED",
            }, 19,
            (Color){255,160,50,255});

        DrawElemenCard(500, 115, 460, 480,
            "Koordinat Grid -> Layar",
            "Konversi sel (col,row) -> piksel",
            (const char*[]){
                "Grid origin: (originX, originY)",
                "Ukuran sel : cellW x cellH",
                "",
                "Posisi pohon di sel (c,r):",
                "  sx = originX + c*cellW",
                "       + cellW/2   <- tengah sel",
                "  sy = originY + r*cellH",
                "       + cellH - 4 <- margin",
                "",
                "Contoh sel (3, 2):",
                "  originX=20, cellW=64",
                "  sx = 20 + 3*64 + 32 = 244",
                "  originY=220, cellH=32",
                "  sy = 220 + 2*32 + 28 = 308",
                "",
                "Konversi mouse -> sel:",
                "  col = (mx-originX) / cellW",
                "  row = (my-originY) / cellH",
                "",
                "Validasi batas:",
                "  0 <= col < GRID_COLS (12)",
                "  0 <= row < GRID_ROWS (8)",
            }, 21,
            (Color){200,150,255,255});
    }

    // ----------------------------------------------------------------
    // Navigasi halaman
    // ----------------------------------------------------------------
    if (page > 0) {
        if (DrawButton(40, SH - 58, 140, 38,
                       (Color){30,60,30,255}, (Color){80,160,60,255},
                       "<", WHITE))
            page--;
    }

    if (page < PAGE_COUNT - 1) {
        if (DrawButton(SW - 180, SH - 58, 140, 38,
                       (Color){30,60,30,255}, (Color){80,160,60,255},
                       ">", WHITE))
            page++;
    }

    // Tombol kembali ke menu
    if (DrawButton(SW/2 - 80, SH - 58, 160, 38,
                   (Color){40,80,40,255}, (Color){100,200,80,255},
                   "Back", WHITE)) {
        EndDrawing();
        page = 0;
        return SCREEN_MENU;
    }

    // Dot indikator halaman
    for (int i = 0; i < PAGE_COUNT; i++) {
        int dotX = SW/2 - (PAGE_COUNT*16)/2 + i*16;
        if (i == page)
            Midcircle(dotX, SH - 15, 5, (Color){150,220,100,255});
        else
            Midcircle(dotX, SH - 15, 4, (Color){60,100,60,200});
    }

    EndDrawing();
    return SCREEN_ELEMEN;
}
