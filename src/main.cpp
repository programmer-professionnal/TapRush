#include "Game.h"
#include <raylib.h>

int main() {
    InitWindow(800, 600, "TapRush - Clicker Game");
    SetTargetFPS(60);

    Game game;
    game.Init();

    while (!WindowShouldClose()) {
        game.Update();
        BeginDrawing();
        ClearBackground({30, 30, 40, 255});
        game.Draw();
        EndDrawing();
    }

    CloseWindow();
    return 0;
}