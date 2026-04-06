#ifndef PARTICLE_H
#define PARTICLE_H

// -----------------------------------------------------------------------------
// Sistem partikel asap untuk pohon terbakar
// Setiap partikel bergerak ke atas + dipengaruhi arah angin
// -----------------------------------------------------------------------------

#define SMOKE_MAX 300   // maksimal partikel aktif sekaligus

typedef struct {
    float x, y;         // posisi layar
    float vx, vy;       // kecepatan (piksel/detik)
    float life;         // 0.0 = mati, 1.0 = baru lahir
    float maxLife;      // durasi hidup total (detik)
    int   size;         // radius partikel (1-4px)
} SmokeParticle;

// Inisialisasi sistem partikel
void ParticleInit(void);

// Spawn partikel asap baru dari posisi (x, y) pohon terbakar
void ParticleSpawn(int x, int y);

// Update semua partikel — gerak + fade
// windDirX, windDirY, windSpeed: dari WindGet*()
void ParticleUpdate(float dt, float windDirX, float windDirY, float windSpeed);

// Gambar semua partikel aktif
void ParticleDraw(void);

// Reset semua partikel (saat GridReset)
void ParticleReset(void);

#endif // PARTICLE_H