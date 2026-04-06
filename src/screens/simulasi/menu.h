#ifndef MENU_H
#define MENU_H

// -----------------------------------------------------------------------------
// Screen menu utama — 4 pilihan: Simulasi, Elemen, Help, About
// Mengembalikan screen berikutnya yang dipilih user
// -----------------------------------------------------------------------------

typedef enum {
    SCREEN_MENU       = 0,
    SCREEN_SIMULASI   = 1,
    SCREEN_ELEMEN     = 2,
    SCREEN_HELP       = 3,
    SCREEN_ABOUT      = 4
} ScreenType;

void       MenuInit(void);
ScreenType MenuUpdate(void);  

#endif 
