#include "Game.h"
#include <raylib.h>

int main() {
    SetConfigFlags(FLAG_VSYNC_HINT | FLAG_MSAA_4X_HINT);
    InitWindow(1280, 720, "TapRush - Clicker Game");
    SetWindowPosition(200, 100);
    SetTargetFPS(60);

    Game game;
    game.Init();

    float saveTimer = 0;
    bool fullscreen = false;
    while (!WindowShouldClose()) {
        game.Update();
        
        if (IsKeyPressed(KEY_F11) || (IsKeyPressed(KEY_ENTER) && IsKeyDown(KEY_LEFT_ALT))) {
            if (fullscreen) {
                SetWindowSize(1280, 720);
                SetWindowPosition(200, 100);
            }
            ToggleFullscreen();
            fullscreen = !fullscreen;
        }
        
        saveTimer += GetFrameTime();
        if (saveTimer >= 30) {
            game.Save();
            saveTimer = 0;
        }
        
        if (IsKeyPressed(KEY_ESCAPE)) {
            game.Save();
            break;
        }
        
        BeginDrawing();
        ClearBackground({20, 25, 35, 255});
        game.Draw();
        EndDrawing();
    }
    
    if (fullscreen) {
        ToggleFullscreen();
    }
    game.Save();
    CloseWindow();
    return 0;
}