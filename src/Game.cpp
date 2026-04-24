#include "Game.h"
#include <raylib.h>
#include <algorithm>

void Game::Init() {
    clickArea = Button(400, 250, 200, 200, "TAP");
    LoadUpgrades();
}

void Game::LoadUpgrades() {
    upgrades = {
        {"Cursor", 15, 1, 0},
        {"Auto Clicker", 100, 1, 0},
        {"Grandma", 500, 5, 0},
        {"Farm", 2000, 20, 0},
        {"Mine", 10000, 100, 0},
        {"Factory", 50000, 500, 0}
    };

    for (size_t i = 0; i < upgrades.size(); i++) {
        float y = 80.0f + i * 50.0f;
        upgradeButtons.emplace_back(600, y, 180, 40, "");
    }
}

void Game::Update() {
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        Rectangle mouseRect = { (float)GetMouseX(), (float)GetMouseY(), 1, 1 };
        if (CheckCollisionRecs(mouseRect, clickArea.GetRect())) {
            Click();
            clickAnimTimer = 0.15f;
        }
    }

    if (clickAnimTimer > 0) {
        clickAnimTimer -= GetFrameTime();
    }

    for (size_t i = 0; i < upgradeButtons.size(); i++) {
        if (upgradeButtons[i].IsClicked()) {
            BuyUpgrade(i);
        }
    }

    int cps = GetClicksPerSecond();
    if (cps > 0) {
        score += cps / 60;
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
        up.cost = (int)(up.cost * 1.15);
        
        if (up.clicksPerSecond > 0) {
            autoClickCps += up.clicksPerSecond;
        } else {
            clickMultiplier += 1;
        }
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

void Game::Draw() {
    DrawClickArea();
    DrawUpgradePanel();
    DrawUI();
}

void Game::DrawClickArea() {
    Color btnColor = {70, 130, 180, 255};
    if (clickAnimTimer > 0) {
        btnColor = {100, 160, 200, 255};
    }
    clickArea.Draw(btnColor, WHITE);
    
    DrawText(TextFormat("+%d", GetClickValue()), 400 - 20, 180, 30, GOLD);
}

void Game::DrawUpgradePanel() {
    DrawRectangle(580, 0, 220, 600, {40, 40, 50, 255});
    DrawLine(580, 0, 580, 600, {100, 100, 120, 255});
    
    DrawText("UPGRADES", 600, 20, 24, GOLD);
    DrawLine(600, 55, 780, 55, {100, 100, 120, 255});

    for (size_t i = 0; i < upgrades.size(); i++) {
        const Upgrade& up = upgrades[i];
        Button& btn = upgradeButtons[i];
        
        bool canAfford = score >= up.cost;
        Color btnColor = canAfford ? Color{60, 60, 80, 255} : Color{30, 30, 40, 255};
        Color textColor = canAfford ? WHITE : GRAY;
        
        btn.Draw(btnColor, textColor);
        
        std::string btnText = TextFormat("%s (%d)", up.name.c_str(), up.count);
        DrawText(btnText.c_str(), btn.GetX() + 10, btn.GetY() + 10, 16, textColor);
        
        std::string costText = TextFormat("%d", up.cost);
        float costWidth = MeasureText(costText.c_str(), 14);
        DrawText(costText.c_str(), btn.GetX() + 170 - costWidth, btn.GetY() + 22, 14, canAfford ? GREEN : RED);
    }
}

void Game::DrawUI() {
    DrawText(TextFormat("Score: %d", score), 20, 20, 32, WHITE);
    DrawText(TextFormat("CPS: %d", autoClickCps), 20, 60, 20, LIGHTGRAY);
    DrawText(TextFormat("Total Clicks: %d", totalClicks), 20, 85, 16, GRAY);
}