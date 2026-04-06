#include "particle.h"
#include "../../../src/algo/bresenham.h"   // BresenhamLine()
#include "raylib.h"
#include <math.h>

static SmokeParticle particles[SMOKE_MAX];

// =============================================================================
// ParticleInit
// =============================================================================
void ParticleInit(void) {
    for (int i = 0; i < SMOKE_MAX; i++)
        particles[i].life = 0.0f;
}

// =============================================================================
// ParticleSpawn — spawn 1 partikel asap dari posisi pohon terbakar
// Dipanggil tiap beberapa frame untuk pohon BURNING
// =============================================================================
void ParticleSpawn(int x, int y) {
    // Cari slot partikel yang mati
    for (int i = 0; i < SMOKE_MAX; i++) {
        if (particles[i].life <= 0.0f) {
            particles[i].x       = (float)x + GetRandomValue(-5, 5);
            particles[i].y       = (float)y;
            // Kecepatan awal: naik ke atas + sedikit menyamping acak
            particles[i].vx      = (float)GetRandomValue(-15, 15);
            particles[i].vy      = (float)GetRandomValue(-40, -20);  // naik
            particles[i].maxLife = 1.5f + (float)GetRandomValue(0, 10) * 0.1f;
            particles[i].life    = particles[i].maxLife;
            particles[i].size    = GetRandomValue(2, 5);
            return;   // spawn 1 per panggilan
        }
    }
}

// =============================================================================
// ParticleUpdate
// Tiap partikel:
//   - Bergerak sesuai velocity
//   - Velocity dipengaruhi angin (drift horizontal)
//   - Melambat seiring waktu (drag)
//   - Life berkurang sampai 0 → mati
// =============================================================================
void ParticleUpdate(float dt, float windDirX, float windDirY, float windSpeed) {
    for (int i = 0; i < SMOKE_MAX; i++) {
        if (particles[i].life <= 0.0f) continue;

        // Kurangi life
        particles[i].life -= dt;
        if (particles[i].life < 0.0f) {
            particles[i].life = 0.0f;
            continue;
        }

        // Pengaruh angin — drift ke arah angin semakin kuat saat partikel naik
        float windForce = windSpeed * 60.0f;
        particles[i].vx += windDirX * windForce * dt;
        particles[i].vy += windDirY * windForce * dt * 0.3f;  // angin vertikal lebih lemah

        // Drag — partikel melambat
        particles[i].vx *= (1.0f - 0.8f * dt);
        particles[i].vy *= (1.0f - 0.3f * dt);

        // Update posisi
        particles[i].x += particles[i].vx * dt;
        particles[i].y += particles[i].vy * dt;
    }
}

// =============================================================================
// ParticleDraw
// Alpha fade: penuh saat baru lahir, transparan saat mendekati mati
// Warna: abu-abu gelap → abu-abu terang → putih transparan
// =============================================================================
void ParticleDraw(void) {
    for (int i = 0; i < SMOKE_MAX; i++) {
        if (particles[i].life <= 0.0f) continue;

        // Rasio hidup: 1.0 = baru lahir, 0.0 = hampir mati
        float ratio = particles[i].life / particles[i].maxLife;

        // Alpha: fade in 0→0.3, penuh 0.3→0.7, fade out 0.7→1.0
        unsigned char alpha;
        if (ratio > 0.7f)
            alpha = (unsigned char)(((1.0f - ratio) / 0.3f) * 160);
        else if (ratio > 0.3f)
            alpha = 160;
        else
            alpha = (unsigned char)((ratio / 0.3f) * 160);

        // Warna berubah: gelap saat baru (dekat api) → terang saat naik
        unsigned char gray = (unsigned char)(60 + (1.0f - ratio) * 140);
        Color smokeCol = (Color){gray, gray, gray, alpha};

        int px = (int)particles[i].x;
        int py = (int)particles[i].y;
        int r  = particles[i].size;

        for (int dy = -r; dy <= r; dy++) {
            float ratio2 = (float)dy / (float)r;
            int dx = (int)(r * sqrtf(1.0f - ratio2 * ratio2));
            BresenhamLine(px - dx, py + dy, px + dx, py + dy, smokeCol);
        }
    }
}

// =============================================================================
// ParticleReset
// =============================================================================
void ParticleReset(void) {
    ParticleInit();
}