#include "menu.h"
#ifndef FIRE_SIM_H
#define FIRE_SIM_H

// -----------------------------------------------------------------------------
// Entry point screen simulasi kebakaran hutan
// Dipanggil dari main.c seperti screen lainnya di repo
// -----------------------------------------------------------------------------

void FireSimInit(void);
ScreenType FireSimUpdate(void);   // update + draw dalam satu fungsi (pola repo ini)
void FireSimUnload(void);

#endif // FIRE_SIM_H
