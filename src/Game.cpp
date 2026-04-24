#include "Game.h"
#include <raylib.h>
#include <algorithm>
#include <fstream>
#include <cmath>

void Game::Init() {
    clickArea = Button(320, 220, 160, 160, "");
    LoadUpgrades();
    Load();
}

void Game::Save() {
    std::ofstream file("save.txt");
    if (file.is_open()) {
        file << score << "\n";
        file << scoreFloat << "\n";
        file << totalClicks << "\n";
        for (const auto& up : upgrades) {
            file << up.count << "\n";
        }
        file.close();
    }
}

void Game::Load() {
    std::ifstream file("save.txt");
    if (file.is_open()) {
        file >> score;
        file >> scoreFloat;
        file >> totalClicks;
        for (auto& up : upgrades) {
            file >> up.count;
        }
        file.close();
    }
}

void Game::LoadUpgrades() {
    upgrades = {
        {"Cursor", 50, -1, 0},
        {"Auto Clicker", 2000, 1, 0},
        {"Grandma", 20000, 5, 0},
        {"Farm", 250000, 20, 0},
        {"Mine", 5000000, 100, 0},
        {"Factory", 100000000, 500, 0}
    };
}

void Game::DrawUpgradePanel() {
    float sw = GetScreenWidth();
    float sh = GetScreenHeight();
    float panelWidth = sw * 0.22f;
    float panelX = sw - panelWidth;
    float btnHeight = sh * 0.055f;
    float btnWidth = panelWidth * 0.88f;
    float startY = sh * 0.11f;
    float spacing = sh * 0.075f;
    
    upgradeButtons.clear();
    for (size_t i = 0; i < upgrades.size(); i++) {
        float y = startY + i * spacing;
        upgradeButtons.emplace_back(panelX + panelWidth * 0.06f, y, btnWidth, btnHeight, "");
    }
    
    DrawRectangle(panelX, 0, panelWidth, sh, {20, 25, 35, 240});
    DrawRectangleLines(panelX, 0, panelWidth, sh, {60, 80, 120, 255});
    
    float titleSize = sh * 0.035f;
    float textSize = sh * 0.025f;
    float smallSize = sh * 0.018f;
    
    DrawText("MEJORAS", panelX + 15, sh * 0.02f, titleSize, {100, 200, 255, 255});
    DrawLine(panelX + 15, sh * 0.065f, sw - 15, sh * 0.065f, {60, 80, 120, 255});

    for (size_t i = 0; i < upgrades.size(); i++) {
        const Upgrade& up = upgrades[i];
        Button& btn = upgradeButtons[i];
        
        bool canAfford = score >= up.cost;
        Color btnColor = canAfford ? Color{40, 50, 80, 255} : Color{25, 30, 45, 255};
        Color textColor = canAfford ? Color{180, 220, 255, 255} : Color{80, 90, 110, 255};
        Color costColor = canAfford ? Color{100, 255, 150, 255} : Color{255, 100, 100, 255};

        btn.Draw(btnColor, textColor);
        
        std::string btnText = TextFormat("%s (%d)", up.name.c_str(), up.count);
        DrawText(btnText.c_str(), btn.GetX() + 10, btn.GetY() + 5, textSize, textColor);
        
        std::string costText = TextFormat("%d", up.cost);
        float costWidth = MeasureText(costText.c_str(), smallSize);
        DrawText(costText.c_str(), btn.GetX() + btnWidth - costWidth - 10, btn.GetY() + btnHeight * 0.5f, smallSize, costColor);
        
        if (up.clicksPerSecond < 0) {
            DrawText("+1/click", btn.GetX() + 10, btn.GetY() + btnHeight - smallSize - 2, smallSize, {150, 150, 170, 200});
        } else if (up.clicksPerSecond > 0) {
            std::string cpsText = TextFormat("+%d/s", up.clicksPerSecond);
            DrawText(cpsText.c_str(), btn.GetX() + 10, btn.GetY() + btnHeight - smallSize - 2, smallSize, {150, 150, 170, 200});
        }
    }
}

void Game::Update() {
    float sw = GetScreenWidth();
    float sh = GetScreenHeight();
    float panelWidth = sw * 0.22f;
    float gameWidth = sw - panelWidth;
    float cx = gameWidth / 2;
    float cy = sh / 2;
    float clickRadius = gameWidth * 0.12f;
    
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        Vector2 mouse = GetMousePosition();
        
        if (mouse.x > cx - clickRadius && mouse.x < cx + clickRadius && mouse.y > cy - clickRadius && mouse.y < cy + clickRadius) {
            Click();
            clickAnimTimer = 0.15f;
        }
    }

    if (clickAnimTimer > 0) {
        clickAnimTimer -= GetFrameTime();
    }

    if (upgradeButtons.size() > 0) {
        for (size_t i = 0; i < upgradeButtons.size(); i++) {
            if (upgradeButtons[i].IsClicked()) {
                BuyUpgrade(i);
            }
        }
    }

    int cps = GetClicksPerSecond();
    if (cps > 0) {
        scoreFloat += cps * GetFrameTime();
        score += (int)scoreFloat;
        scoreFloat -= (int)scoreFloat;
    }
}

void Game::Click() {
    score += GetClickValue();
    totalClicks++;
}

void Game::BuyUpgrade(int index) {
    if (index < 0 || index >= (int)upgrades.size()) return;
    
    Upgrade& up = upgrades[index];
    if (score >= up.cost) {
        score -= up.cost;
        up.count++;
        up.cost = (int)(up.cost * 1.5);
    }
}

int Game::GetClicksPerSecond() const {
    int cps = 0;
    for (const auto& up : upgrades) {
        if (up.clicksPerSecond > 0) {
            cps += up.count * up.clicksPerSecond;
        }
    }
    return cps;
}

int Game::GetCursorCount() const {
    int cursors = 0;
    for (const auto& up : upgrades) {
        if (up.clicksPerSecond < 0) {
            cursors += up.count;
        }
    }
    return cursors;
}

void Game::Draw() {
    DrawClickArea();
    DrawUpgradePanel();
    DrawUI();
}

void Game::DrawClickArea() {
    float sw = GetScreenWidth();
    float sh = GetScreenHeight();
    float panelWidth = sw * 0.22f;
    float gameWidth = sw - panelWidth;
    Vector2 center = {gameWidth / 2, sh / 2};
    float size = clickAnimTimer > 0 ? gameWidth * 0.08f : gameWidth * 0.1f;
    
    Color starColor = {50, 150, 255, 255};
    Color glowColor = {50, 150, 255, 100};
    
    DrawCircleV(center, size + size * 0.3f, glowColor);
    DrawCircleV(center, size + size * 0.15f, {glowColor.r, glowColor.g, glowColor.b, 150});
    
    for (int i = 0; i < 6; i++) {
        float angle = (i * 60 - 90) * DEG2RAD;
        float outerX = center.x + cosf(angle) * size;
        float outerY = center.y + sinf(angle) * size;
        DrawLine(center.x, center.y, outerX, outerY, starColor);
    }
    
    DrawPoly(center, 6, size, -90, starColor);
    DrawPoly(center, 6, size * 0.5f, -90, {30, 30, 40, 255});
    
    float textSize = gameWidth * 0.04f;
    DrawText(TextFormat("+%d", GetClickValue()), center.x - textSize * 0.7f, center.y - size - textSize, textSize, GOLD);
    DrawText("TAP!", center.x - textSize * 0.5f, center.y + size + textSize * 0.3f, textSize * 0.5f, {150, 150, 170, 255});
}

void Game::DrawUI() {
    float sw = GetScreenWidth();
    float sh = GetScreenHeight();
    float panelWidth = sw * 0.22f;
    float headerHeight = sh * 0.1f;
    float gameWidth = sw - panelWidth;
    
    DrawRectangle(0, 0, gameWidth, headerHeight, {15, 20, 35, 230});
    DrawRectangleLines(0, 0, gameWidth, headerHeight, {50, 80, 120, 255});
    
    float fontSize = sh * 0.035f;
    float smallFontSize = sh * 0.02f;
    
    DrawText("TAP RUSH", 20, 12, fontSize, {100, 200, 255, 255});
    DrawText(TextFormat("%d", score), 20, 48, fontSize, {255, 255, 255, 255});
    
    DrawText(TextFormat("CPS: %d", GetClicksPerSecond()), gameWidth * 0.35f, sh * 0.055f, smallFontSize, {150, 200, 255, 200});
    DrawText(TextFormat("Clicks: %d", totalClicks), gameWidth * 0.55f, sh * 0.055f, smallFontSize, {150, 150, 170, 200});
}