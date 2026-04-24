#include "Game.h"
#include <raylib.h>
#include <algorithm>
#include <fstream>
#include <cmath>

void Game::Init() {
    LoadUpgrades();
    LoadAchievements();
    LoadMissions();
    Load();
    event.active = false;
    event.timer = 0;
    event.multiplier = 1;
}

void Game::SpawnClickParticles(Vector2 pos, int count) {
    for (int i = 0; i < count; i++) {
        Particle p;
        p.pos = pos;
        float angle = (float)(GetRandomValue(0, 360) * DEG2RAD);
        float speed = (float)GetRandomValue(50, 200);
        p.vel = {cosf(angle) * speed, sinf(angle) * speed};
        p.life = 1.0f;
        p.maxLife = 1.0f;
        p.color = Color{100, 200, 255, 255};
        p.size = (float)GetRandomValue(3, 10);
        particles.push_back(p);
    }
}

void Game::SpawnFloatText(Vector2 pos, const char* text, Color color) {
    Particle ft;
    ft.pos = pos;
    ft.vel = {0, -80};
    ft.life = 1.5f;
    ft.maxLife = 1.5f;
    ft.color = color;
    ft.size = 30;
    floatTexts.push_back(ft);
}

void Game::UpdateParticles() {
    for (auto& p : particles) {
        p.pos.x += p.vel.x * GetFrameTime();
        p.pos.y += p.vel.y * GetFrameTime();
        p.vel.y += 200 * GetFrameTime();
        p.life -= GetFrameTime();
    }
    particles.erase(std::remove_if(particles.begin(), particles.end(), 
        [](const Particle& p) { return p.life <= 0; }), particles.end());

    for (auto& ft : floatTexts) {
        ft.pos.y += ft.vel.y * GetFrameTime();
        ft.life -= GetFrameTime();
    }
    floatTexts.erase(std::remove_if(floatTexts.begin(), floatTexts.end(), 
        [](const Particle& p) { return p.life <= 0; }), floatTexts.end());
}

void Game::DrawParticles() {
    for (const auto& p : particles) {
        float alpha = p.life / p.maxLife;
        Color c = p.color;
        c.a = (unsigned char)(255 * alpha);
        DrawCircleV(p.pos, p.size * alpha, c);
    }
    
    float sw = GetScreenWidth();
    float sh = GetScreenHeight();
    float panelW = sw * 0.22f;
    float gameW = sw - panelW;
    float textSize = gameW * 0.04f;
    
    for (const auto& ft : floatTexts) {
        float alpha = ft.life / ft.maxLife;
        int size = (int)(ft.size * alpha);
        Color c = ft.color;
        c.a = (unsigned char)(255 * alpha);
        DrawText(TextFormat("+%d", GetClickValue()), (int)ft.pos.x - 20, (int)ft.pos.y, size, c);
    }
}

void Game::LoadUpgrades() {
    upgrades = {
        {"Cursor", 50, -1, 0, false, 0},
        {"Auto Clicker", 2000, 1, 0, false, 0},
        {"Grandma", 20000, 5, 0, false, 0},
        {"Farm", 250000, 20, 0, false, 0},
        {"Mine", 5000000, 100, 0, false, 0},
        {"Factory", 100000000, 500, 0, false, 0}
    };
}

void Game::LoadAchievements() {
    achievements = {
        {"Primer Click", "Haz tu primer clic", false, 0},
        {"Dedazos", "1000 clics", false, 0},
        {"Maestro", "10000 clics", false, 0},
        {"Mejora 1", "Primera mejora", false, 0},
        {"Coleccionista", "5 mejoras", false, 0},
        {"Combo x5", "Alcanza combo x5", false, 0},
        {"Productor", "100 CPS", false, 0},
        {"Nivel 5", "Alcanza nivel 5", false, 0}
    };
}

void Game::LoadMissions() {
    missions = {
        {"Click 100", "100 clics", 100, 0, false, 500},
        {"Click 1000", "1000 clics", 1000, 0, false, 5000},
        {"Ganar 1K", "1000 puntos", 1000, 0, false, 1000}
    };
    dailyMissions = {};
}

void Game::AddXP(int amount) {
    xp += amount * level;
    CheckLevelUp();
}

void Game::CheckLevelUp() {
    int xpNeeded = level * 1000;
    while (xp >= xpNeeded) {
        xp -= xpNeeded;
        level++;
        xpNeeded = level * 1000;
        
        float sw = GetScreenWidth();
        float sh = GetScreenHeight();
        float panelW = sw * 0.22f;
        float gW = sw - panelW;
        SpawnFloatText({gW / 2, sh * 0.3f}, TextFormat("NIVEL %d!", level), {255, 215, 0, 255});
    }
}

void Game::SpawnRandomEvent() {
    if (event.active || GetRandomValue(0, 100) > 5) return;
    
    int eventType = GetRandomValue(0, 3);
    event.active = true;
    event.timer = 60;
    
    switch (eventType) {
        case 0: event.name = "2x Puntos"; event.multiplier = 2; break;
        case 1: event.name = "3x Puntos"; event.multiplier = 3; break;
        case 2: event.name = "5x Puntos"; event.multiplier = 5; break;
        default: event.name = "Doble Click"; event.multiplier = 2; break;
    }
    
    float sw = GetScreenWidth();
    float sh = GetScreenHeight();
    float panelW = sw * 0.22f;
    float gW = sw - panelW;
    SpawnFloatText({gW / 2, sh * 0.25f}, TextFormat("EVENTO: %s!", event.name.c_str()), {255, 50, 50, 255});
}

void Game::CheckMissions() {
    for (auto& m : missions) {
        if (!m.completed) {
            if (m.name.find("Click") != std::string::npos) m.current = totalClicks;
            else if (m.name.find("Ganar") != std::string::npos) m.current = score;
            
            if (m.current >= m.target) {
                m.completed = true;
                score += m.reward;
                AddXP(m.reward / 10);
            }
        }
    }
}

void Game::CheckAchievements() {
    int totalUpgrades = 0;
    for (auto& up : upgrades) totalUpgrades += up.count;
    
    for (size_t i = 0; i < achievements.size(); i++) {
        if (achievements[i].unlocked) continue;
        
        bool unlock = false;
        switch (i) {
            case 0: unlock = totalClicks >= 1; break;
            case 1: unlock = totalClicks >= 1000; break;
            case 2: unlock = totalClicks >= 10000; break;
            case 3: unlock = totalUpgrades >= 1; break;
            case 4: unlock = totalUpgrades >= 5; break;
            case 5: unlock = combo >= 5; break;
            case 6: unlock = GetClicksPerSecond() >= 100; break;
            case 7: unlock = level >= 5; break;
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
    AddXP(100 * (index + 1));
}

void Game::DrawAchievements() {
    if (achievementTimer <= 0) return;
    
    float sw = GetScreenWidth();
    float sh = GetScreenHeight();
    float alpha = achievementTimer / 3.0f;
    Color bg = {0, 0, 0, (unsigned char)(180 * alpha)};
    Color textC = {255, 215, 0, (unsigned char)(255 * alpha)};
    
    float notifW = sw * 0.4f;
    float notifH = sh * 0.08f;
    float notifX = (sw - notifW) / 2;
    
    DrawRectangle((int)notifX, 20, (int)notifW, (int)notifH, bg);
    DrawRectangleLines((int)notifX, 20, (int)notifW, (int)notifH, textC);
    
    for (auto& a : achievements) {
        if (a.unlocked && GetTime() - a.unlockTime < achievementTimer) {
            DrawText(TextFormat("✓ %s", a.name.c_str()), (int)(notifX + 20), (int)(20 + notifH * 0.3f), (int)(sh * 0.04f), textC);
            break;
        }
    }
}

void Game::DrawMissions() {
    float sw = GetScreenWidth();
    float sh = GetScreenHeight();
    float panelW = sw * 0.22f;
    float panelX = sw - panelW;
    float startY = sh * 0.72f;
    float fontSize = sh * 0.018f;
    
    DrawText("MISIONES", panelX + 15, startY - 35, fontSize + 2, {100, 200, 255, 255});
    
    int shown = 0;
    for (auto& m : missions) {
        if (shown >= 3) break;
        
        float y = startY + shown * (sh * 0.08f);
        Color mColor = m.completed ? Color{100, 255, 150, 255} : Color{150, 150, 170, 255};
        
        DrawRectangle(panelX + 10, (int)y, (int)(panelW - 20), (int)(sh * 0.06f), {30, 35, 50, 200});
        DrawText(TextFormat("%s: %d/%d", m.name.c_str(), m.current, m.target), panelX + 15, y + 5, fontSize, mColor);
        
        if (m.completed) {
            DrawText("✓", panelX + panelW - 35, y + 5, fontSize + 2, Color{100, 255, 150, 255});
        }
        shown++;
    }
    for (auto& m : dailyMissions) {
        if (shown >= 4) break;
        
        float y = startY + shown * (sh * 0.08f);
        Color mColor = m.completed ? Color{100, 255, 150, 255} : Color{150, 150, 170, 255};
        
        DrawRectangle(panelX + 10, (int)y, (int)(panelW - 20), (int)(sh * 0.06f), {30, 35, 50, 200});
        DrawText(TextFormat("%s", m.name.c_str()), panelX + 15, y + 5, fontSize, mColor);
        shown++;
    }
}

void Game::DrawCombo() {
    if (combo <= 0) return;
    
    float sw = GetScreenWidth();
    float sh = GetScreenHeight();
    float panelW = sw * 0.22f;
    float gameW = sw - panelW;
    float fontSize = sh * 0.03f;
    
    float pulse = sinf(GetTime() * 10) * 0.1f + 1.0f;
    Color comboColor = {255, (unsigned char)(150 + combo * 10), 0, 255};
    
    DrawText(TextFormat("COMBO x%d!", combo), (int)(gameW / 2 - MeasureText(TextFormat("COMBO x%d!", combo), (int)fontSize) / 2), 
           (int)(sh * 0.22f), (int)(fontSize * pulse), comboColor);
}

void Game::DrawEvent() {
    if (!event.active) return;
    
    float sw = GetScreenWidth();
    float sh = GetScreenHeight();
    float fontSize = sh * 0.025f;
    
    float alpha = sinf(GetTime() * 5) * 0.3f + 0.7f;
    Color eventColor = {255, 50, 50, (unsigned char)(255 * alpha)};
    
    DrawText(TextFormat("%s - %.0fs", event.name.c_str(), event.timer), (int)(sw * 0.35f), (int)(sh * 0.85f), (int)fontSize, eventColor);
}

void Game::DrawLevel() {
    float sw = GetScreenWidth();
    float sh = GetScreenHeight();
    float panelW = sw * 0.22f;
    float gameW = sw - panelW;
    float fontSize = sh * 0.025f;
    
    int xpNeeded = level * 1000;
    float xpPercent = (float)xp / xpNeeded;
    
    std::string levelText = TextFormat("Nivel %d", level);
    DrawText(levelText.c_str(), (int)(gameW * 0.3f), (int)(sh * 0.035f), (int)fontSize, {100, 200, 255, 255});
    
    float barW = gameW * 0.25f;
    float barH = sh * 0.015f;
    float barX = gameW * 0.3f + MeasureText(levelText.c_str(), (int)fontSize) + 10;
    float barY = sh * 0.04f;
    
    DrawRectangle((int)barX, (int)barY, (int)barW, (int)barH, {50, 50, 70, 255});
    DrawRectangle((int)barX, (int)barY, (int)(barW * xpPercent), (int)barH, {100, 200, 255, 255});
}

void Game::DrawStats() {
    float sw = GetScreenWidth();
    float sh = GetScreenHeight();
    float panelW = sw * 0.22f;
    float panelX = sw - panelW;
    float startY = sh * 0.55f;
    float fontSize = sh * 0.018f;
    
    DrawText("ESTADISTICAS", panelX + 15, startY, fontSize + 2, {100, 200, 255, 255});
    DrawLine((int)panelX + 15, (int)(startY + fontSize + 3), (int)(sw - 15), (int)(startY + fontSize + 3), {60, 80, 120, 255});
    
    DrawText(TextFormat("Clicks: %d", totalClicks), panelX + 15, startY + fontSize + 8, fontSize, {150, 150, 170, 255});
    DrawText(TextFormat("Puntos: %d", score), panelX + 15, startY + fontSize + 25, fontSize, {150, 150, 170, 255});
    DrawText(TextFormat("Mayor: x%d", combo), panelX + 15, startY + fontSize + 42, fontSize, {150, 150, 170, 255});
}

void Game::Save() {
    std::ofstream file("save.txt");
    if (file.is_open()) {
        file << score << "\n" << scoreFloat << "\n" << totalClicks << "\n" << level << "\n" << xp << "\n";
        for (const auto& up : upgrades) file << up.count << "\n";
        for (const auto& a : achievements) file << (a.unlocked ? 1 : 0) << "\n";
        file.close();
    }
}

void Game::Load() {
    std::ifstream file("save.txt");
    if (file.is_open()) {
        file >> score >> scoreFloat >> totalClicks >> level >> xp;
        for (auto& up : upgrades) file >> up.count;
        for (auto& a : achievements) {
            int unlock; file >> unlock; a.unlocked = unlock == 1;
        }
        file.close();
    }
}

void Game::Update() {
    float sw = GetScreenWidth(), sh = GetScreenHeight();
    float panelWidth = sw * 0.22f, gameWidth = sw - panelWidth;
    float cx = gameWidth / 2, cy = sh / 2, clickRadius = gameWidth * 0.12f;
    
    UpdateParticles();
    starRotation += GetFrameTime() * 30;
    pulseTimer += GetFrameTime() * 2;
    
    if (achievementTimer > 0) achievementTimer -= GetFrameTime();
    
    comboTimer += GetFrameTime();
    if (comboTimer >= 2.0f) {
        if (combo > 0) combo = 0;
    }
    
    eventTimer += GetFrameTime();
    if (eventTimer >= 30) {
        eventTimer = 0;
        SpawnRandomEvent();
    }
    
    if (event.active) {
        event.timer -= GetFrameTime();
        if (event.timer <= 0) event.active = false;
    }
    
    autoClickTimer += GetFrameTime();
    int cps = GetClicksPerSecond();
    if (cps > 0 && autoClickTimer >= 1.0f) {
        autoClickTimer = 0;
        score += cps;
        float gW = sw - panelWidth;
        SpawnFloatText({gW / 2, sh / 2 - 80}, TextFormat("+%d", cps), {100, 200, 255, 255});
    }
    
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        Vector2 mouse = GetMousePosition();
        
        if (mouse.x > cx - clickRadius && mouse.x < cx + clickRadius && 
            mouse.y > cy - clickRadius && mouse.y < cy + clickRadius) {
            Click();
            clickAnimTimer = 0.15f;
            combo++;
            comboTimer = 0;
        }
    }

    if (clickAnimTimer > 0) clickAnimTimer -= GetFrameTime();

    for (size_t i = 0; i < upgradeButtons.size(); i++) {
        if (upgradeButtons[i].IsClicked()) {
            BuyUpgrade(i);
        }
    }

    CheckAchievements();
    CheckMissions();

    float scorePerSec = GetClicksPerSecond() * GetFrameTime();
    if (scorePerSec > 0) {
        scoreFloat += scorePerSec;
        score += (int)scoreFloat;
        scoreFloat -= (int)scoreFloat;
    }
}

void Game::Click() {
    int clickValue = GetClickValue();
    score += clickValue;
    totalClicks++;
    AddXP(1);
    
    if (event.name == "Duble Click") score += clickValue;
    
    float sw = GetScreenWidth(), sh = GetScreenHeight();
    float panelWidth = sw * 0.22f, gameWidth = sw - panelWidth;
    Vector2 center = {gameWidth / 2, sh / 2};
    
    SpawnClickParticles(center, 5 + combo);
    SpawnFloatText({center.x, center.y - 50}, TextFormat("+%d", clickValue), GOLD);
}

void Game::BuyUpgrade(int index) {
    if (index < 0 || index >= (int)upgrades.size()) return;
    Upgrade& up = upgrades[index];
    if (score >= up.cost) {
        score -= up.cost;
        up.count++;
        up.cost = (int)(up.cost * 1.5);
        AddXP(100);
        CheckAchievements();
        CheckMissions();
    }
}

int Game::GetClicksPerSecond() const {
    int cps = 0;
    for (const auto& up : upgrades) {
        if (up.clicksPerSecond > 0) cps += up.count * up.clicksPerSecond;
    }
    return cps * event.multiplier;
}

int Game::GetCursorCount() const {
    int cursors = 0;
    for (const auto& up : upgrades) {
        if (up.clicksPerSecond < 0) cursors += up.count;
    }
    return cursors;
}

void Game::Draw() {
    DrawClickArea();
    DrawParticles();
    DrawUpgradePanel();
    DrawUI();
    DrawAchievements();
    DrawMissions();
    DrawCombo();
    DrawEvent();
    DrawLevel();
    DrawStats();
}

void Game::DrawClickArea() {
    float sw = GetScreenWidth(), sh = GetScreenHeight();
    float panelWidth = sw * 0.22f, gameWidth = sw - panelWidth;
    Vector2 center = {gameWidth / 2, sh / 2};
    float baseSize = gameWidth * 0.1f;
    float size = clickAnimTimer > 0 ? baseSize * 0.8f : baseSize;
    float pulse = sinf(pulseTimer) * 0.1f + 1.0f;
    
    Color starColor = event.active ? Color{255, 100, 100, 255} : Color{50, 150, 255, 255};
    Color glowColor = event.active ? Color{255, 50, 50, 80} : Color{50, 150, 255, 80};
    
    float glowSize = size * (1.3f + pulse * 0.2f);
    DrawCircleV(center, glowSize + size * 0.3f, glowColor);
    DrawCircleV(center, glowSize + size * 0.15f, {glowColor.r, glowColor.g, glowColor.b, 120});
    
    float rotation = starRotation * DEG2RAD;
    for (int i = 0; i < 6; i++) {
        float angle = (i * 60) + rotation;
        DrawLine(center.x, center.y, center.x + cosf(angle) * size, center.y + sinf(angle) * size, starColor);
    }
    
    DrawPoly(center, 6, size, rotation * RAD2DEG, starColor);
    DrawPoly(center, 6, size * 0.5f, rotation * RAD2DEG, {30, 30, 40, 255});
    
    float textSize = gameWidth * 0.04f;
    DrawText(TextFormat("+%d", GetClickValue()), center.x - textSize * 0.7f, center.y - size - textSize, textSize, GOLD);
    DrawText("TAP!", center.x - textSize * 0.5f, center.y + size + textSize * 0.3f, textSize * 0.5f, {150, 150, 170, 255});
}

void Game::DrawUI() {
    float sw = GetScreenWidth(), sh = GetScreenHeight();
    float panelWidth = sw * 0.22f, headerHeight = sh * 0.1f, gameWidth = sw - panelWidth;
    
    DrawRectangle(0, 0, gameWidth, headerHeight, {15, 20, 35, 230});
    DrawRectangleLines(0, 0, gameWidth, headerHeight, {50, 80, 120, 255});
    
    float fontSize = sh * 0.035f, smallFontSize = sh * 0.02f;
    
    DrawText("TAP RUSH", 20, 12, fontSize, {100, 200, 255, 255});
    DrawText(TextFormat("%d", score), 20, 48, fontSize, {255, 255, 255, 255});
    DrawText(TextFormat("CPS: %d", GetClicksPerSecond()), gameWidth * 0.35f, sh * 0.055f, smallFontSize, {150, 200, 255, 200});
    DrawText(TextFormat("Clicks: %d", totalClicks), gameWidth * 0.55f, sh * 0.055f, smallFontSize, {150, 150, 170, 200});
}

void Game::DrawUpgradePanel() {
    float sw = GetScreenWidth(), sh = GetScreenHeight();
    float panelWidth = sw * 0.22f, panelX = sw - panelWidth;
    float btnHeight = sh * 0.055f, btnWidth = panelWidth * 0.88f;
    float startY = sh * 0.11f, spacing = sh * 0.065f;
    
    upgradeButtons.clear();
    for (size_t i = 0; i < upgrades.size(); i++) {
        float y = startY + i * spacing;
        if (y + btnHeight > sh * 0.55f) break;
        upgradeButtons.emplace_back(panelX + panelWidth * 0.06f, y, btnWidth, btnHeight, "");
    }
    
    DrawRectangle(panelX, 0, panelWidth, sh, {20, 25, 35, 240});
    DrawRectangleLines(panelX, 0, panelWidth, sh, {60, 80, 120, 255});
    
    float textSize = sh * 0.025f, smallSize = sh * 0.018f;
    
    DrawText("MEJORAS", panelX + 15, sh * 0.02f, textSize, {100, 200, 255, 255});
    DrawLine((int)panelX + 15, (int)(sh * 0.065f), (int)(sw - 15), (int)(sh * 0.065f), {60, 80, 120, 255});

    for (size_t i = 0; i < upgradeButtons.size(); i++) {
        const Upgrade& up = upgrades[i];
        Button& btn = upgradeButtons[i];
        
        bool canAfford = score >= up.cost;
        Color btnColor = canAfford ? Color{40, 50, 80, 255} : Color{25, 30, 45, 255};
        Color textColor = canAfford ? Color{180, 220, 255, 255} : Color{80, 90, 110, 255};
        Color costColor = canAfford ? Color{100, 255, 150, 255} : Color{255, 100, 100, 255};

        btn.Draw(btnColor, textColor);
        
        DrawText(TextFormat("%s (%d)", up.name.c_str(), up.count), btn.GetX() + 10, btn.GetY() + 5, textSize, textColor);
        
        std::string costText = TextFormat("%d", up.cost);
        float costW = MeasureText(costText.c_str(), (int)smallSize);
        DrawText(costText.c_str(), btn.GetX() + btnWidth - costW - 10, btn.GetY() + btnHeight * 0.5f, smallSize, costColor);
        
        if (up.clicksPerSecond < 0) {
            DrawText("+1/click", btn.GetX() + 10, btn.GetY() + btnHeight - smallSize - 2, smallSize, {150, 150, 170, 200});
        } else if (up.clicksPerSecond > 0) {
            DrawText(TextFormat("+%d/s", up.clicksPerSecond), btn.GetX() + 10, btn.GetY() + btnHeight - smallSize - 2, smallSize, {150, 150, 170, 200});
        }
    }
}