#include "Game.h"
#include <raylib.h>
#include <algorithm>
#include <fstream>
#include <cmath>
#include <ctime>

int Game::GetDayOfYear() {
    time_t now = time(nullptr);
    tm* local = localtime(&now);
    int day = local->tm_yday;
    return day;
}

void Game::UpdateStreakMultiplier() {
    if (dailyStreak >= 7) streakMultiplier = 7;
    else if (dailyStreak >= 3) streakMultiplier = dailyStreak;
    else streakMultiplier = 1;
}

void Game::Init() {
    LoadUpgrades();
    LoadAchievements();
    LoadMissions();
    LoadWeeklyMissions();
    Load();
    event.active = false;
    event.timer = 0;
    event.multiplier = 1;
    specialEventActive = false;
    specialEventName = "";
    
    for (int i = 0; i < 10; i++) {
        LeaderboardEntry e;
        e.name = TextFormat("Player %d", i + 1);
        e.score = GetRandomValue(1000, 1000000);
        e.level = GetRandomValue(1, 50);
        e.prestige = GetRandomValue(0, 3);
        leaderboard.push_back(e);
    }
    
    CheckDailyReset();
    CheckWeeklyReset();
    
    float sw = GetScreenWidth(), sh = GetScreenHeight();
    float panelW = sw * 0.22f, panelX = sw - panelW;
    float tabHeight = sh * 0.07f;
    float tabWidth = panelW / 4;
    
    menuTabs.clear();
    menuTabs.emplace_back(panelX, 0, tabWidth, tabHeight, " Mej.");
    menuTabs.emplace_back(panelX + tabWidth, 0, tabWidth, tabHeight, " Mis.");
    menuTabs.emplace_back(panelX + tabWidth * 2, 0, tabWidth, tabHeight, " Prog.");
    menuTabs.emplace_back(panelX + tabWidth * 3, 0, tabWidth, tabHeight, " Est.");
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
    particles.erase(std::remove_if(particles.begin(), particles.end(), [](const Particle& p) { return p.life <= 0; }), particles.end());
    for (auto& ft : floatTexts) {
        ft.pos.y += ft.vel.y * GetFrameTime();
        ft.life -= GetFrameTime();
    }
    floatTexts.erase(std::remove_if(floatTexts.begin(), floatTexts.end(), [](const Particle& p) { return p.life <= 0; }), floatTexts.end());
}

void Game::DrawParticles() {
    for (const auto& p : particles) {
        float alpha = p.life / p.maxLife;
        Color c = p.color;
        c.a = (unsigned char)(255 * alpha);
        DrawCircleV(p.pos, p.size * alpha, c);
    }
    float sw = GetScreenWidth(), sh = GetScreenHeight();
    float panelW = sw * 0.22f, gameW = sw - panelW;
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
        {"Cursor", 100, -1, 0, false, 0},
        {"Auto Clicker", 5000, 1, 0, false, 0},
        {"Grandma", 50000, 5, 0, false, 0},
        {"Farm", 750000, 20, 0, false, 0},
        {"Mine", 15000000, 100, 0, false, 0},
        {"Factory", 300000000, 500, 0, false, 0},
        {"Temple", 5000000000LL, 2500, 0, false, 0},
        {"Magic Portal", 75000000000LL, 10000, 0, false, 0},
        {"Quantum Core", 1000000000000LL, 50000, 0, false, 0},
        {"Alien Tech", 25000000000000LL, 250000, 0, false, 0}
    };
}

void Game::LoadAchievements() {
    achievements = {
        {"Principiante", "100 clics", false, 0},
        {"Aprendiz", "500 clics", false, 0},
        {"Intermedio", "1000 clics", false, 0},
        {"Avanzado", "5000 clics", false, 0},
        {"Experto", "10000 clics", false, 0},
        {"Maestro Clicker", "50000 clics", false, 0},
        {"Leyenda", "100000 clics", false, 0},
        {"leyendario", "500000 clics", false, 0},
        {"Primera Compra", "Primera mejora", false, 0},
        {"Comprador", "5 mejoras", false, 0},
        {"Coleccionista", "10 mejoras", false, 0},
        {"Acumulador", "25 mejoras", false, 0},
        {"Magnate", "50 mejoras", false, 0},
        {"Combo x3", "Combo x3", false, 0},
        {"Combo x5", "Combo x5", false, 0},
        {"Combo x10", "Combo x10", false, 0},
        {"Combo x25", "Combo x25", false, 0},
        {"Combo x50", "Combo x50", false, 0},
        {"Nivel 5", "Nivel 5", false, 0},
        {"Nivel 10", "Nivel 10", false, 0},
        {"Nivel 25", "Nivel 25", false, 0},
        {"Nivel 50", "Nivel 50", false, 0},
        {"Productor Basico", "10 CPS", false, 0},
        {"Productor", "100 CPS", false, 0},
        {"Productor Avanzado", "1000 CPS", false, 0},
        {"Productor Experto", "10000 CPS", false, 0},
        {"Millonario", "1M puntos", false, 0},
        {" multimillonario", "10M puntos", false, 0},
        {"Gigamillonario", "100M puntos", false, 0},
        {"Evento Lucky", "Activa un evento", false, 0}
    };
}

void Game::LoadMissions() {
    missions = {
        {"Click 100", "100 clics", 100, 0, false, 500},
        {"Click 500", "500 clics", 500, 0, false, 2000},
        {"Click 1000", "1000 clics", 1000, 0, false, 5000},
        {"Click 5000", "5000 clics", 5000, 0, false, 25000},
        {"Click 10K", "10000 clics", 10000, 0, false, 50000},
        {"Click 50K", "50000 clics", 50000, 0, false, 100000},
        {"Ganar 1K", "1000 puntos", 1000, 0, false, 1000},
        {"Ganar 10K", "10000 puntos", 10000, 0, false, 5000},
        {"Ganar 100K", "100000 puntos", 100000, 0, false, 10000},
        {"Ganar 500K", "500000 puntos", 500000, 0, false, 50000},
        {"Ganar 1M", "1000000 puntos", 1000000, 0, false, 100000},
        {"Ganar 5M", "5000000 puntos", 5000000, 0, false, 250000},
        {"Combo x5", "Combo x5", 5, 0, false, 1000},
        {"Combo x10", "Combo x10", 10, 0, false, 2500},
        {"Combo x25", "Combo x25", 25, 0, false, 5000},
        {"Combo x50", "Combo x50", 50, 0, false, 10000},
        {"Combo x100", "Combo x100", 100, 0, false, 25000},
        {"CPS 10", "10 CPS", 10, 0, false, 2000},
        {"CPS 100", "100 CPS", 100, 0, false, 10000},
        {"CPS 1K", "1000 CPS", 1000, 0, false, 50000}
    };
    dailyMissions = {
        {"Daily: 500 Clics", "500 clics hoy", 500, 0, false, 2500},
        {"Daily: 2K Clics", "2000 clics hoy", 2000, 0, false, 5000},
        {"Daily: 5K Pts", "5000 pts hoy", 5000, 0, false, 3000},
        {"Daily: 10K Pts", "10000 pts hoy", 10000, 0, false, 5000},
        {"Daily: 100 CPS", "100 CPS", 100, 0, false, 10000},
        {"Daily: 500 CPS", "500 CPS", 500, 0, false, 15000}
    };
}

void Game::LoadWeeklyMissions() {
    weeklyMissions = {
        {"Semanal: 10K Clics", "10000 clics esta semana", 10000, 0, false, 50000, false},
        {"Semanal: 100K Pts", "100000 pts esta semana", 100000, 0, false, 100000, false},
        {"Semanal: Nivel 10", "Alcanza nivel 10", 10, 0, false, 150000, false},
        {"Semanal: 500 CPS", "500 CPS", 500, 0, false, 200000, false},
        {"Semanal: Compras", "10 mejoras", 10, 0, false, 250000, false}
    };
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
        levelUpAnim = 2.0f;
        screenShake = 0.2f;
        float sw = GetScreenWidth(), sh = GetScreenHeight();
        float panelW = sw * 0.22f;
        SpawnFloatText({sw / 2 - panelW / 2, sh * 0.3f}, TextFormat("NIVEL %d!", level), {255, 215, 0, 255});
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
    float sw = GetScreenWidth(), sh = GetScreenHeight();
    SpawnFloatText({sw * 0.39f, sh * 0.25f}, TextFormat("EVENTO: %s!", event.name.c_str()), {255, 50, 50, 255});
}

void Game::CheckMissions() {
    for (auto& m : missions) {
        if (!m.completed) {
            if (m.name.find("Click") != std::string::npos) m.current = totalClicks;
            else if (m.name.find("Ganar") != std::string::npos) m.current = score;
            else if (m.name.find("Combo") != std::string::npos) m.current = combo;
            if (m.current >= m.target) {
                m.completed = true;
                score += m.reward;
                weeklyScore += m.reward;
                AddXP(m.reward / 10);
            }
        }
    }
    for (auto& m : dailyMissions) {
        if (!m.completed) {
            if (m.name.find("Click") != std::string::npos) m.current = totalClicks;
            else if (m.name.find("Pts") != std::string::npos || m.name.find("pts") != std::string::npos) m.current = score;
            else if (m.name.find("CPS") != std::string::npos) m.current = GetClicksPerSecond();
            if (m.current >= m.target) {
                m.completed = true;
                score += m.reward;
                weeklyScore += m.reward;
                AddXP(m.reward / 10);
            }
        }
    }
}

void Game::CheckWeeklyMissions() {
    for (auto& m : weeklyMissions) {
        if (!m.completed && !m.claimed) {
            if (m.name.find("Clics") != std::string::npos) m.current = weeklyClicks;
            else if (m.name.find("Pts") != std::string::npos || m.name.find("pts") != std::string::npos) m.current = weeklyScore;
            else if (m.name.find("Nivel") != std::string::npos) m.current = level;
            else if (m.name.find("CPS") != std::string::npos) m.current = GetClicksPerSecond();
            else if (m.name.find("Compras") != std::string::npos) {
                int totalUpgrades = 0;
                for (auto& up : upgrades) totalUpgrades += up.count;
                m.current = totalUpgrades;
            }
            if (m.current >= m.target && !m.completed) {
                m.completed = true;
            }
        }
    }
}

void Game::CheckDailyReset() {
    int currentDay = GetDayOfYear();
    if (lastPlayDay != currentDay) {
        if (currentDay == lastPlayDay + 1 || (lastPlayDay == 365 && currentDay == 0)) {
            AddStreak();
        } else {
            dailyStreak = 0;
            streakMultiplier = 1;
            AddStreak();
        }
        
        for (auto& m : dailyMissions) {
            m.current = 0;
            m.completed = false;
        }
        lastPlayDay = currentDay;
    }
}

void Game::CheckWeeklyReset() {
    int currentDay = GetDayOfYear();
    if (currentDay < weeklyStartDay || currentDay - weeklyStartDay >= 7) {
        weeklyScore = 0;
        weeklyClicks = 0;
        weeklyUpgrades = 0;
        weeklyStartDay = currentDay;
        for (auto& wm : weeklyMissions) {
            wm.current = 0;
            wm.completed = false;
            wm.claimed = false;
        }
    }
}

void Game::AddWeeklyProgress(const std::string& type, int amount) {
    if (type == "score") {
        weeklyScore += amount;
        for (auto& m : weeklyMissions) if (m.name.find("Pts") != std::string::npos) {
            m.current = weeklyScore;
            if (m.current >= m.target && !m.completed) m.completed = true;
        }
    }
}

void Game::SpawnSpecialEvent() {
    if (specialEventActive || GetRandomValue(0, 100) > 3) return;
    int eventType = GetRandomValue(0, 2);
    specialEventActive = true;
    specialEventTimer = 120;
    switch (eventType) {
        case 0: specialEventName = "Lucky Hour"; break;
        case 1: specialEventName = "Mega Combo"; break;
        case 2: specialEventName = "Click Frenzy"; break;
    }
    float sw = GetScreenWidth(), sh = GetScreenHeight();
    SpawnFloatText({sw * 0.39f, sh * 0.35f}, TextFormat("ESPECIAL: %s!", specialEventName.c_str()), {255, 100, 0, 255});
}

void Game::Prestige() {
    int prestigeScore = score + totalClicks * 10 + GetClicksPerSecond() * 100;
    if (prestigeScore < 10000000) {
        SpawnFloatText({(float)GetScreenWidth() / 2, (float)GetScreenHeight() / 2}, "Necesitas 10M puntos!", {255, 50, 50, 255});
        return;
    }
    prestigeLevel++;
    prestigeMultiplier = 1 + prestigeLevel;
    score = 0;
    totalClicks = 0;
    level = 1;
    xp = 0;
    for (auto& up : upgrades) {
        up.count = 0;
    }
    for (auto& m : missions) {
        m.completed = false;
        m.current = 0;
    }
    for (auto& m : dailyMissions) {
        m.completed = false;
        m.current = 0;
    }
    screenShake = 0.5f;
    SpawnFloatText({(float)GetScreenWidth() / 2, GetScreenHeight() * 0.3f}, TextFormat("PRESTIGIO %d!", prestigeLevel), {255, 215, 0, 255});
    AddXP(1000 * prestigeLevel);
}

void Game::CheckAchievements() {
    int totalUpgrades = 0;
    for (auto& up : upgrades) totalUpgrades += up.count;
    for (size_t i = 0; i < achievements.size(); i++) {
        if (achievements[i].unlocked) continue;
        bool unlock = false;
        switch (i) {
            case 0: unlock = totalClicks >= 100; break;
            case 1: unlock = totalClicks >= 500; break;
            case 2: unlock = totalClicks >= 1000; break;
            case 3: unlock = totalClicks >= 5000; break;
            case 4: unlock = totalClicks >= 10000; break;
            case 5: unlock = totalClicks >= 50000; break;
            case 6: unlock = totalClicks >= 100000; break;
            case 7: unlock = totalClicks >= 500000; break;
            case 8: unlock = totalUpgrades >= 1; break;
            case 9: unlock = totalUpgrades >= 5; break;
            case 10: unlock = totalUpgrades >= 10; break;
            case 11: unlock = totalUpgrades >= 25; break;
            case 12: unlock = totalUpgrades >= 50; break;
            case 13: unlock = combo >= 3; break;
            case 14: unlock = combo >= 5; break;
            case 15: unlock = combo >= 10; break;
            case 16: unlock = combo >= 25; break;
            case 17: unlock = combo >= 50; break;
            case 18: unlock = level >= 5; break;
            case 19: unlock = level >= 10; break;
            case 20: unlock = level >= 25; break;
            case 21: unlock = level >= 50; break;
            case 22: unlock = GetClicksPerSecond() >= 10; break;
            case 23: unlock = GetClicksPerSecond() >= 100; break;
            case 24: unlock = GetClicksPerSecond() >= 1000; break;
            case 25: unlock = GetClicksPerSecond() >= 10000; break;
            case 26: unlock = score >= 1000000; break;
            case 27: unlock = score >= 10000000; break;
            case 28: unlock = score >= 100000000; break;
            case 29: unlock = event.active; break;
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
    float sw = GetScreenWidth(), sh = GetScreenHeight();
    SpawnFloatText({sw * 0.39f, sh * 0.15f}, TextFormat("LOGRO: %s!", achievements[index].name.c_str()), {255, 215, 0, 255});
    AddXP(100 * (index + 1));
}

void Game::DrawAchievements() {
    if (achievementTimer <= 0) return;
    float sw = GetScreenWidth(), sh = GetScreenHeight();
    float alpha = achievementTimer / 3.0f;
    Color bg = {0, 0, 0, (unsigned char)(180 * alpha)};
    Color textC = {255, 215, 0, (unsigned char)(255 * alpha)};
    float notifW = sw * 0.4f, notifH = sh * 0.08f, notifX = (sw - notifW) / 2;
    DrawRectangle((int)notifX, 20, (int)notifW, (int)notifH, bg);
    DrawRectangleLines((int)notifX, 20, (int)notifW, (int)notifH, textC);
    for (auto& a : achievements) {
        if (a.unlocked && GetTime() - a.unlockTime < achievementTimer) {
            DrawText(TextFormat("✓ %s", a.name.c_str()), (int)(notifX + 20), (int)(20 + notifH * 0.3f), (int)(sh * 0.04f), textC);
            break;
        }
    }
}

void Game::DrawMenuTabs() {
    float sw = GetScreenWidth(), sh = GetScreenHeight();
    float panelW = sw * 0.22f, panelX = sw - panelW;
    float tabHeight = sh * 0.07f;
    float tabWidth = panelW / 4;
    
    DrawRectangle(panelX, 0, panelW, tabHeight, {20, 25, 35, 240});
    DrawRectangleLines(panelX, 0, panelW, tabHeight, {60, 80, 120, 255});
    
    for (size_t i = 0; i < menuTabs.size(); i++) {
        bool isActive = (currentTab == (MenuTab)i);
        Color tabColor = isActive ? Color{40, 50, 80, 255} : Color{25, 30, 45, 255};
        Color textColor = isActive ? Color{180, 220, 255, 255} : Color{120, 130, 150, 255};
        if (menuTabs[i].IsClicked()) currentTab = (MenuTab)i;
        menuTabs[i].Draw(tabColor, textColor);
    }
}

void Game::DrawMissionsPanel() {
    float sw = GetScreenWidth(), sh = GetScreenHeight();
    float panelW = sw * 0.22f, panelX = sw - panelW;
    float tabHeight = sh * 0.07f;
    float startY = tabHeight + 10;
    float fontSize = sh * 0.018f;
    
    DrawText("MISIONES", panelX + 15, startY, fontSize + 2, {100, 200, 255, 255});
    DrawLine((int)panelX + 15, (int)(startY + fontSize + 3), (int)(sw - 15), (int)(startY + fontSize + 3), {60, 80, 120, 255});
    
    int missionHeight = (int)((sh - startY - 30) / 2);
    int shown = 0;
    for (auto& m : missions) {
        if (shown >= 4) break;
        float y = startY + fontSize + 10 + shown * missionHeight;
        Color mColor = m.completed ? Color{100, 255, 150, 255} : Color{150, 150, 170, 255};
        DrawRectangle(panelX + 10, (int)y, (int)(panelW - 20), (int)(missionHeight - 5), {30, 35, 50, 200});
        DrawText(TextFormat("%s", m.name.c_str()), panelX + 15, y + 5, fontSize, mColor);
        DrawText(TextFormat("%d/%d", m.current, m.target), panelX + 15, y + fontSize + 8, fontSize * 0.8f, mColor);
        if (m.completed) DrawText("✓", panelX + panelW - 35, y + 5, fontSize + 2, Color{100, 255, 150, 255});
        shown++;
    }
    
    float dailyY = startY + fontSize + 10 + 4 * missionHeight + 5;
    DrawText("DIARIAS", panelX + 15, dailyY - fontSize - 5, fontSize, {255, 200, 100, 255});
    
    shown = 0;
    for (auto& m : dailyMissions) {
        if (shown >= 4) break;
        float y = dailyY + shown * missionHeight;
        Color mColor = m.completed ? Color{100, 255, 150, 255} : Color{150, 150, 170, 255};
        DrawRectangle(panelX + 10, (int)y, (int)(panelW - 20), (int)(missionHeight - 5), {40, 35, 50, 200});
        DrawText(TextFormat("%s", m.name.c_str()), panelX + 15, y + 5, fontSize, mColor);
        shown++;
    }
}

void Game::DrawProgressPanel() {
    float sw = GetScreenWidth(), sh = GetScreenHeight();
    float panelW = sw * 0.22f, panelX = sw - panelW;
    float tabHeight = sh * 0.07f;
    float startY = tabHeight + 10;
    float fontSize = sh * 0.018f;
    int unlockedCount = 0;
    for (auto& a : achievements) if (a.unlocked) unlockedCount++;
    
    DrawText("PROGRESO", panelX + 15, startY, fontSize + 2, {100, 200, 255, 255});
    DrawLine((int)panelX + 15, (int)(startY + fontSize + 3), (int)(sw - 15), (int)(startY + fontSize + 3), {60, 80, 120, 255});
    
    float barW = panelW * 0.85f;
    float barH = fontSize + 5;
    DrawText(TextFormat("Nivel %d", level), panelX + 15, startY + fontSize + 15, fontSize + 4, {100, 200, 255, 255});
    DrawRectangle(panelX + 15, startY + fontSize * 2 + 25, (int)barW, (int)barH, {40, 40, 60, 255});
    int xpNeeded = level * 1000;
    DrawRectangle(panelX + 15, startY + fontSize * 2 + 25, (int)(barW * (float)xp / xpNeeded), (int)barH, {50, 150, 255, 255});
    DrawText(TextFormat("%d/%d XP", xp, xpNeeded), panelX + 15, startY + fontSize * 2 + 25 + barH + 3, fontSize * 0.8f, {150, 150, 170, 255});
    
    float progY = startY + fontSize * 2 + 25 + barH + 20;
    DrawText(TextFormat("Logros: %d/%d", unlockedCount, (int)achievements.size()), panelX + 15, progY, fontSize, {255, 215, 0, 255});
    
    for (size_t i = 0; i < achievements.size(); i++) {
        float y = progY + fontSize + 10 + i * (fontSize + 2);
        if (y > sh - 20) break;
        Color aColor = achievements[i].unlocked ? Color{100, 255, 150, 255} : Color{80, 80, 100, 255};
        DrawText(TextFormat("%s%s", achievements[i].unlocked ? "✓ " : "○ ", achievements[i].name.c_str()), panelX + 15, y, fontSize * 0.85f, aColor);
    }
}

void Game::DrawStatsPanel() {
    float sw = GetScreenWidth(), sh = GetScreenHeight();
    float panelW = sw * 0.22f, panelX = sw - panelW;
    float tabHeight = sh * 0.07f;
    float startY = tabHeight + 10;
    float fontSize = sh * 0.018f;
    
    DrawText("ESTADISTICAS", panelX + 15, startY, fontSize + 2, {100, 200, 255, 255});
    DrawLine((int)panelX + 15, (int)(startY + fontSize + 3), (int)(sw - 15), (int)(startY + fontSize + 3), {60, 80, 120, 255});
    
    std::vector<std::string> stats = {
        TextFormat("Puntuacion: %d", score),
        TextFormat("Clicks Totales: %d", totalClicks),
        TextFormat("CPS: %d", GetClicksPerSecond()),
        TextFormat("Mayor Combo: x%d", combo),
        TextFormat("Nivel: %d", level),
        TextFormat("XP Total: %.0f", xp)
    };
    
    for (size_t i = 0; i < stats.size(); i++) {
        float y = startY + fontSize + 15 + i * (fontSize + 10);
        DrawText(stats[i].c_str(), panelX + 15, y, fontSize, {150, 150, 170, 255});
    }
}

void Game::DrawCombo() {
    if (combo <= 0) return;
    float sw = GetScreenWidth(), sh = GetScreenHeight();
    float panelW = sw * 0.22f, gameW = sw - panelW;
    float fontSize = sh * 0.03f;
    float pulse = sinf(GetTime() * 10) * 0.1f + 1.0f;
    Color comboColor = {255, (unsigned char)(150 + combo * 10), 0, 255};
    DrawText(TextFormat("COMBO x%d!", combo), (int)(gameW / 2 - MeasureText(TextFormat("COMBO x%d!", combo), (int)fontSize) / 2), (int)(sh * 0.22f), (int)(fontSize * pulse), comboColor);
}

void Game::DrawEvent() {
    if (!event.active) return;
    float sw = GetScreenWidth(), sh = GetScreenHeight();
    float fontSize = sh * 0.025f;
    float alpha = sinf(GetTime() * 5) * 0.3f + 0.7f;
    Color eventColor = {255, 50, 50, (unsigned char)(255 * alpha)};
    DrawText(TextFormat("%s - %.0fs", event.name.c_str(), event.timer), (int)(sw * 0.35f), (int)(sh * 0.85f), (int)fontSize, eventColor);
}

void Game::DrawLevel() {
    float sw = GetScreenWidth(), sh = GetScreenHeight();
    float panelW = sw * 0.22f, gameW = sw - panelW;
    float fontSize = sh * 0.022f;
    int xpNeeded = level * 1000;
    std::string levelText = TextFormat("Nivel %d", level);
    DrawText(levelText.c_str(), (int)(gameW * 0.5f), 15, (int)fontSize, {100, 200, 255, 255});
    float barW = gameW * 0.15f, barH = sh * 0.012f;
    float barX = gameW * 0.5f + MeasureText(levelText.c_str(), (int)fontSize) + 10, barY = 22;
    DrawRectangle((int)barX, (int)barY, (int)barW, (int)barH, {50, 50, 70, 255});
    DrawRectangle((int)barX, (int)barY, (int)(barW * (float)xp / xpNeeded), (int)barH, {100, 200, 255, 255});
}

void Game::DrawStats() {
    float sw = GetScreenWidth(), sh = GetScreenHeight();
    float panelW = sw * 0.22f, panelX = sw - panelW;
    float startY = sh * 0.54f;
    float fontSize = sh * 0.018f;
    DrawText(TextFormat("Clicks: %d", totalClicks), panelX + 15, startY, fontSize, {150, 150, 170, 255});
    DrawText(TextFormat("Puntos: %d", score), panelX + 15, startY + fontSize + 12, fontSize, {150, 150, 170, 255});
    DrawText(TextFormat("Mayor: x%d", combo), panelX + 15, startY + fontSize * 2 + 24, fontSize, {150, 150, 170, 255});
}

void Game::Save() {
    std::ofstream file("save.txt");
    if (file.is_open()) {
        file << score << "\n" << scoreFloat << "\n" << totalClicks << "\n" << level << "\n" << xp << "\n";
        file << energy << "\n" << dailyStreak << "\n" << lastPlayDay << "\n";
        file << prestigeLevel << "\n" << prestigeMultiplier << "\n";
        file << weeklyScore << "\n" << weeklyClicks << "\n" << weeklyUpgrades << "\n" << weeklyStartDay << "\n";
        for (const auto& up : upgrades) file << up.count << "\n";
        for (const auto& a : achievements) file << (a.unlocked ? 1 : 0) << "\n";
        for (const auto& m : dailyMissions) file << m.current << "\n" << m.completed << "\n";
        for (const auto& wm : weeklyMissions) file << wm.current << "\n" << wm.completed << "\n" << wm.claimed << "\n";
        file.close();
    }
}

void Game::Load() {
    std::ifstream file("save.txt");
    if (file.is_open()) {
        file >> score >> scoreFloat >> totalClicks >> level >> xp;
        file >> energy >> dailyStreak >> lastPlayDay;
        file >> prestigeLevel >> prestigeMultiplier;
        file >> weeklyScore >> weeklyClicks >> weeklyUpgrades >> weeklyStartDay;
        for (auto& up : upgrades) file >> up.count;
        for (auto& a : achievements) { int unlock; file >> unlock; a.unlocked = unlock == 1; }
        for (auto& m : dailyMissions) { file >> m.current >> m.completed; }
        for (auto& wm : weeklyMissions) { file >> wm.current >> wm.completed >> wm.claimed; }
        file.close();
    }
}

void Game::Update() {
    float sw = GetScreenWidth(), sh = GetScreenHeight();
    float panelWidth = sw * 0.22f, gameWidth = sw - panelWidth;
    float cx = gameWidth / 2, cy = sh / 2, clickRadius = gameWidth * 0.12f;
    
    if (screenShake > 0) screenShake -= GetFrameTime();
    if (levelUpAnim > 0) levelUpAnim -= GetFrameTime();
    
    UpdateParticles();
    starRotation += GetFrameTime() * 30;
    pulseTimer += GetFrameTime() * 2;
    if (achievementTimer > 0) achievementTimer -= GetFrameTime();
    
    comboTimer += GetFrameTime();
    if (comboTimer >= 2.0f) if (combo > 0) combo = 0;
    
    eventTimer += GetFrameTime();
    if (eventTimer >= 30) { eventTimer = 0; SpawnRandomEvent(); }
    
    specialEventTimer += GetFrameTime();
    if (specialEventTimer >= 120) { specialEventTimer = 0; SpawnSpecialEvent(); }
    
    if (event.active) {
        event.timer -= GetFrameTime();
        if (event.timer <= 0) event.active = false;
    }
    
    if (specialEventActive) {
        specialEventTimer -= GetFrameTime();
        if (specialEventTimer <= 0) specialEventActive = false;
    }
    
    autoClickTimer += GetFrameTime();
    int cps = GetClicksPerSecond();
    if (cps > 0 && autoClickTimer >= 1.0f) {
        autoClickTimer = 0;
        score += cps;
        SpawnFloatText({gameWidth / 2, sh / 2 - 80}, TextFormat("+%d", cps), {100, 200, 255, 255});
    }
    
    energyRegenTimer += GetFrameTime();
    if (energyRegenTimer >= 1.0f && energy < maxEnergy) {
        energyRegenTimer = 0;
        energy++;
    }
    
    CheckDailyReset();
    CheckWeeklyReset();
    
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        Vector2 mouse = GetMousePosition();
        if (mouse.x > cx - clickRadius && mouse.x < cx + clickRadius && mouse.y > cy - clickRadius && mouse.y < cy + clickRadius) {
            Click();
            clickAnimTimer = 0.15f;
            combo++;
            comboTimer = 0;
        }
    }

    if (clickAnimTimer > 0) clickAnimTimer -= GetFrameTime();

    for (size_t i = 0; i < upgradeButtons.size(); i++) {
        if (upgradeButtons[i].IsClicked()) BuyUpgrade(i);
    }

    CheckAchievements();
    CheckMissions();
    CheckWeeklyMissions();

    float scorePerSec = GetClicksPerSecond() * GetFrameTime();
    if (scorePerSec > 0) {
        scoreFloat += scorePerSec;
        score += (int)scoreFloat;
        scoreFloat -= (int)scoreFloat;
    }
}

void Game::Click() {
    if (!CanClick()) {
        SpawnFloatText({(float)GetScreenWidth() / 2, (float)GetScreenHeight() / 2}, "SIN ENERGIA!", {255, 50, 50, 255});
        return;
    }
    
    int clickValue = GetClickValue();
    score += clickValue;
    totalClicks++;
    weeklyClicks++;
    UseEnergy();
    AddXP(1);
    if (event.name == "Doble Click") score += clickValue;
    float sw = GetScreenWidth(), sh = GetScreenHeight();
    float panelWidth = sw * 0.22f, gameWidth = sw - panelWidth;
    Vector2 center = {gameWidth / 2, sh / 2};
    SpawnClickParticles(center, 5 + combo);
    
    int particles = 5 + combo;
    if (specialEventActive && specialEventName == "Click Frenzy") particles *= 3;
    SpawnClickParticles(center, particles);
    
    SpawnFloatText({center.x, center.y - 50}, TextFormat("+%d", clickValue), GOLD);
    
    if (specialEventActive && specialEventName == "Click Frenzy") {
        SpawnFloatText({center.x + 50, center.y - 80}, TextFormat("+%d", clickValue), {255, 100, 0, 255});
    }
}

void Game::BuyUpgrade(int index) {
    if (index < 0 || index >= (int)upgrades.size()) return;
    Upgrade& up = upgrades[index];
    if ((long long)score >= up.cost) {
        score -= (int)up.cost;
        up.count++;
        up.cost = (long long)(up.cost * 1.6);
        AddXP(100);
        CheckAchievements();
        CheckMissions();
    }
}

int Game::GetClicksPerSecond() const {
    int cps = 0;
    for (const auto& up : upgrades) if (up.clicksPerSecond > 0) cps += up.count * up.clicksPerSecond;
    return cps * event.multiplier;
}

int Game::GetCursorCount() const {
    int cursors = 0;
    for (const auto& up : upgrades) if (up.clicksPerSecond < 0) cursors += up.count;
    return cursors;
}

void Game::Draw() {
    DrawClickArea();
    DrawParticles();
    DrawUI();
    DrawMenuTabs();
    DrawEnergy();
    DrawStreak();
    DrawPrestige();
    DrawSpecialEvent();
    
    switch (currentTab) {
        case TAB_UPGRADES: DrawUpgradePanel(); break;
        case TAB_MISSIONS: DrawMissionsPanel(); break;
        case TAB_PROGRESS: DrawProgressPanel(); break;
        case TAB_STATS: DrawStatsPanel(); break;
    }
    
    DrawAchievements();
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
    DrawRectangle(0, 0, gameWidth, headerHeight, {15,20,35,230});
    DrawRectangleLines(0, 0, gameWidth, headerHeight, {50,80,120,255});
    
    float fontSize = sh * 0.04f, smallFontSize = sh * 0.018f;
    DrawText("TAP RUSH", 20, 12, fontSize, {100, 200, 255, 255});
    
    long long displayScore = (long long)score * prestigeMultiplier;
    std::string scoreStr;
    if (displayScore >= 1000000000) scoreStr = TextFormat("%.1fB", (float)displayScore / 1000000000.0f);
    else if (displayScore >= 1000000) scoreStr = TextFormat("%.1fM", (float)displayScore / 1000000.0f);
    else if (displayScore >= 1000) scoreStr = TextFormat("%.1fK", (float)displayScore / 1000.0f);
    else scoreStr = TextFormat("%lld", displayScore);
    DrawText(scoreStr.c_str(), 20, 52, fontSize, {255, 255, 255, 255});
    
    DrawText(TextFormat("CPS: %d", GetClicksPerSecond()), gameWidth * 0.35f, 15, smallFontSize, {150, 200, 255, 200});
    DrawText(TextFormat("Clicks: %lld", totalClicks), gameWidth * 0.55f, 15, smallFontSize, {150, 150, 170, 200});
    DrawText(TextFormat("Combo: x%d", combo), gameWidth * 0.75f, 15, smallFontSize, {255, 180, 0, 255});
}

void Game::DrawEnergy() {
    float sw = GetScreenWidth(), sh = GetScreenHeight();
    float panelW = sw * 0.22f, gameW = sw - panelW;
    float fontSize = sh * 0.018f;
    float barW = gameW * 0.15f, barH = sh * 0.012f;
    float barX = gameW * 0.35f, barY = sh * 0.085f;
    
    Color energyColor = energy > 100 ? Color{50, 200, 100, 255} : (energy > 50 ? Color{200, 200, 50, 255} : Color{200, 50, 50, 255});
    DrawRectangle((int)barX, (int)barY, (int)barW, (int)barH, {30, 30, 40, 255});
    DrawRectangle((int)barX, (int)barY, (int)(barW * (float)energy / maxEnergy), (int)barH, energyColor);
    DrawRectangleLines((int)barX, (int)barY, (int)barW, (int)barH, {80, 80, 100, 255});
    DrawText(TextFormat("Energia: %d/%d", energy, maxEnergy), (int)barX, (int)(barY - fontSize - 2), (int)fontSize, energyColor);
}

void Game::DrawStreak() {
    float sw = GetScreenWidth(), sh = GetScreenHeight();
    float panelW = sw * 0.22f, gameW = sw - panelW;
    float fontSize = sh * 0.018f;
    Color streakColor = {255, 150, 0, 255};
    DrawText(TextFormat("Racha: %d dias (x%d)", dailyStreak, streakMultiplier), (int)(gameW * 0.55f), (int)(sh * 0.085f), (int)fontSize, streakColor);
}

void Game::DrawPrestige() {
    if (prestigeLevel <= 0) return;
    float sw = GetScreenWidth(), sh = GetScreenHeight();
    float fontSize = sh * 0.02f;
    Color prestigeColor = {255, 215, 0, 255};
    DrawText(TextFormat("Prestigio %d (x%d)", prestigeLevel, prestigeMultiplier), 20, (int)(sh * 0.105f), (int)fontSize, prestigeColor);
}

void Game::DrawSpecialEvent() {
    if (!specialEventActive) return;
    float sw = GetScreenWidth(), sh = GetScreenHeight();
    float fontSize = sh * 0.025f;
    float alpha = specialEventTimer / 120.0f;
    Color eventColor = {255, 100, 0, (unsigned char)(255 * alpha)};
    DrawText(TextFormat("%s: %.0fs", specialEventName.c_str(), specialEventTimer), (int)(sw * 0.3f), (int)(sh * 0.15f), (int)fontSize, eventColor);
}

void Game::DrawLeaderboard() {
    if (currentTab != TAB_STATS) return;
    float sw = GetScreenWidth(), sh = GetScreenHeight();
    float panelW = sw * 0.22f, panelX = sw - panelW;
    float startY = sh * 0.4f;
    float fontSize = sh * 0.018f;
    
    DrawText("Top 10", (int)panelX + 10, (int)startY, (int)fontSize, {255, 215, 0, 255});
    for (size_t i = 0; i < leaderboard.size() && i < 10; i++) {
        Color rankColor = leaderboard[i].name == "Tú" ? Color{100, 255, 150, 255} : Color{180, 180, 200, 255};
        std::string entryText = TextFormat("%d. %s: %d (Nv.%d)", i + 1, leaderboard[i].name.c_str(), leaderboard[i].score, leaderboard[i].level);
        DrawText(entryText.c_str(), (int)panelX + 10, (int)(startY + fontSize * 1.5f * (i + 1)), (int)(fontSize * 0.9f), rankColor);
    }
}

void Game::DrawUpgradePanel() {
    float sw = GetScreenWidth(), sh = GetScreenHeight();
    float panelWidth = sw * 0.22f, panelX = sw - panelWidth;
    float tabHeight = sh * 0.07f;
    float btnHeight = sh * 0.055f, btnWidth = panelWidth * 0.88f;
    float startY = tabHeight + 15, spacing = sh * 0.06f;
    
    upgradeButtons.clear();
    for (size_t i = 0; i < upgrades.size(); i++) {
        float y = startY + i * spacing;
        if (y + btnHeight > sh - 20) break;
        upgradeButtons.emplace_back(panelX + panelWidth * 0.06f, y, btnWidth, btnHeight, "");
    }
    
    for (size_t i = 0; i < upgradeButtons.size(); i++) {
        const Upgrade& up = upgrades[i];
        Button& btn = upgradeButtons[i];
        bool canAfford = (long long)score >= up.cost;
        Color btnColor = canAfford ? Color{40, 50, 80, 255} : Color{25, 30, 45, 255};
        Color textColor = canAfford ? Color{180, 220, 255, 255} : Color{80, 90, 110, 255};
        Color costColor = canAfford ? Color{100, 255, 150, 255} : Color{255, 100, 100, 255};
        btn.Draw(btnColor, textColor);
        
        float textSize = sh * 0.022f, smallSize = sh * 0.016f;
        DrawText(TextFormat("%s (%d)", up.name.c_str(), up.count), btn.GetX() + 10, btn.GetY() + 5, textSize, textColor);
        
        long long displayCost = up.cost;
        std::string costText;
        if (displayCost >= 1000000000) costText = TextFormat("%.1fB", (float)displayCost / 1000000000.0f);
        else if (displayCost >= 1000000) costText = TextFormat("%.1fM", (float)displayCost / 1000000.0f);
        else if (displayCost >= 1000) costText = TextFormat("%.1fK", (float)displayCost / 1000.0f);
        else costText = TextFormat("%lld", displayCost);
        
        float costW = MeasureText(costText.c_str(), (int)smallSize);
        DrawText(costText.c_str(), btn.GetX() + btnWidth - costW - 10, btn.GetY() + btnHeight * 0.5f, smallSize, costColor);
        
        if (up.clicksPerSecond < 0) DrawText("+1/click", btn.GetX() + 10, btn.GetY() + btnHeight - smallSize - 2, smallSize, {150, 150, 170, 200});
        else if (up.clicksPerSecond > 0) DrawText(TextFormat("+%d/s", up.clicksPerSecond), btn.GetX() + 10, btn.GetY() + btnHeight - smallSize - 2, smallSize, {150, 150, 170, 200});
    }
}