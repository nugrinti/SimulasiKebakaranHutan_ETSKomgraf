#include "raylib.h"
#include "src/screens/simulasi/menu.h"
#include "src/screens/simulasi/fire_simulation.h"
#include "src/screens/help/help.h"
#include "src/screens/about/about.h"
#include "src/screens/elemen/elemen.h"

int main(void) {
    InitWindow(1000, 700, "Simulasi Kebakaran Hutan 2D");
    SetTargetFPS(60);

    MenuInit();
    FireSimInit();

    ScreenType currentScreen = SCREEN_MENU;  // ← mulai dari menu

    while (!WindowShouldClose()) {
        switch (currentScreen) {
            case SCREEN_MENU:     currentScreen = MenuUpdate();    break;
            case SCREEN_SIMULASI: currentScreen = FireSimUpdate(); break;
            case SCREEN_HELP:     currentScreen = HelpUpdate();    break;
            case SCREEN_ABOUT:    currentScreen = AboutUpdate();   break;
            case SCREEN_ELEMEN:   currentScreen = ElemenUpdate();  break;
        }
    }

    FireSimUnload();
    CloseWindow();
    return 0;
}