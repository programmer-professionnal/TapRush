#include "Game.h"
#include <raylib.h>

int main() {
    SetConfigFlags(FLAG_VSYNC_HINT | FLAG_MSAA_4X_HINT);
    InitWindow(1920, 1080, "TapRush - Clicker Game");
    SetTargetFPS(60);

    Game game;
    game.Init();

    float saveTimer = 0;
    bool fullscreen = false;
    while (!WindowShouldClose()) {
        game.Update();
        
        if (IsKeyPressed(KEY_F11) || (IsKeyPressed(KEY_ENTER) && IsKeyDown(KEY_LEFT_ALT))) {
            if (fullscreen) {
                SetWindowSize(1920, 1080);
                SetWindowPosition(100, 50);
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