#include "Game.h"
#include <raylib.h>
#include <cstddef>

void Game::LoadAchievements() {
    achievements = {
        {"Primer click", "Haz tu primer clic", false, 0},
        {"10 clics", "Haz 10 clics", false, 0},
        {"100 clics", "Haz 100 clics", false, 0},
        {"1000 clics", "Haz 1000 clics", false, 0},
        {"Primera mejora", "Compra tu primera mejora", false, 0},
        {"Mejora x10", "Compra 10 mejoras", false, 0},
        {"Primera estrella", "Llega a +5 por clic", false, 0},
        {"Productor", "Alcanza 10 CPS", false, 0},
        {"Imparable", "Alcanza 100 CPS", false, 0},
        {"Puntuación 1K", "Llega a 1000 puntos", false, 0},
        {"Puntuación 10K", "Llega a 10000 puntos", false, 0},
        {"Puntuación 100K", "Llega a 100000 puntos", false, 0}
    };
}

void Game::CheckAchievements() {
    for (size_t i = 0; i < achievements.size(); i++) {
        if (achievements[i].unlocked) continue;
        
        bool unlock = false;
        switch (i) {
            case 0: unlock = totalClicks >= 1; break;
            case 1: unlock = totalClicks >= 10; break;
            case 2: unlock = totalClicks >= 100; break;
            case 3: unlock = totalClicks >= 1000; break;
            case 4:
                for (auto& up : upgrades) if (up.count > 0) unlock = true;
                break;
            case 5: {
                int total = 0;
                for (auto& up : upgrades) total += up.count;
                unlock = total >= 10;
                break;
            }
            case 6: unlock = GetClickValue() >= 5; break;
            case 7: unlock = GetClicksPerSecond() >= 10; break;
            case 8: unlock = GetClicksPerSecond() >= 100; break;
            case 9: unlock = score >= 1000; break;
            case 10: unlock = score >= 10000; break;
            case 11: unlock = score >= 100000; break;
        }
        
        if (unlock) UnlockAchievement(i);
    }
}

void Game::UnlockAchievement(int index) {
    if (index < 0 || index >= (int)achievements.size()) return;
    if (achievements[index].unlocked) return;
    
    achievements[index].unlocked = true;
    achievements[index].unlockTime = (float)GetTime();
    achievementTimer = 3.0f;
    
    float sw = GetScreenWidth();
    float sh = GetScreenHeight();
    float panelW = sw * 0.22f;
    float gW = sw - panelW;
    SpawnFloatText({gW / 2, sh * 0.15f}, TextFormat("LOGRO: %s!", achievements[index].name.c_str()), {255, 215, 0, 255});
}

void Game::DrawAchievements() {
    if (achievementTimer <= 0) return;
    
    float sw = GetScreenWidth();
    float sh = GetScreenHeight();
    float panelW = sw * 0.22f;
    
    float alpha = achievementTimer / 3.0f;
    Color bg = {0, 0, 0, (unsigned char)(180 * alpha)};
    Color textC = {255, 215, 0, (unsigned char)(255 * alpha)};
    
    float notifW = sw * 0.4f;
    float notifH = sh * 0.08f;
    float notifX = (sw - notifW) / 2;
    float notifY = 20;
    
    DrawRectangle((int)notifX, (int)notifY, (int)notifW, (int)notifH, bg);
    DrawRectangleLines((int)notifX, (int)notifY, (int)notifW, (int)notifH, textC);
    
    for (auto& a : achievements) {
        if (a.unlocked && GetTime() - a.unlockTime < achievementTimer) {
            float nameSize = sh * 0.04f;
            DrawText(TextFormat("✓ %s", a.name.c_str()), (int)(notifX + 20), (int)(notifY + notifH * 0.3f), (int)nameSize, textC);
            break;
        }
    }
}