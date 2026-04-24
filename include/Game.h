#pragma once
#include <raylib.h>
#include <vector>
#include <string>
#include "Button.h"

struct Upgrade {
    std::string name;
    int cost;
    int clicksPerSecond;
    int count;
    bool isLimited;
    float limitedTime;
};

struct Mission {
    std::string name;
    std::string description;
    int target;
    int current;
    bool completed;
    int reward;
    bool isDaily;
};

struct Particle {
    Vector2 pos;
    Vector2 vel;
    float life;
    float maxLife;
    Color color;
    float size;
};

struct Achievement {
    std::string name;
    std::string description;
    bool unlocked;
    float unlockTime;
};

struct Event {
    std::string name;
    float duration;
    float timer;
    bool active;
    int multiplier;
};

class Game {
public:
    void Init();
    void Update();
    void Draw();
    void Save();
    void Load();

    void AddUpgrade(const std::string& name, int baseCost, int cps);
    void BuyUpgrade(int index);
    void Click();

    int GetScore() const { return score; }
    int GetClicksPerSecond() const;
    int GetClickValue() const {
        int base = 1 + combo;
        for (const auto& up : upgrades) {
            if (up.clicksPerSecond < 0) {
                base += up.count;
            }
        }
        if (event.active) base *= event.multiplier;
        return base;
    }
    int GetCursorCount() const;
    int GetLevel() const { return level; }
    float GetXP() const { return xp; }
    float GetCombo() const { return combo; }

private:
    int score = 0;
    float scoreFloat = 0;
    int totalClicks = 0;
    int level = 1;
    float xp = 0;
    int combo = 0;
    float comboTimer = 0;
    std::vector<Upgrade> upgrades;
    std::vector<Button> upgradeButtons;
    std::vector<Particle> particles;
    std::vector<Particle> floatTexts;
    std::vector<Achievement> achievements;
    std::vector<Mission> missions;
    std::vector<Mission> dailyMissions;

    Button clickArea;
    float clickAnimTimer = 0;
    float starRotation = 0;
    float pulseTimer = 0;
    float autoClickTimer = 0;
    float achievementTimer = 0;
    float dailyResetTimer = 0;
    float eventTimer = 0;
    
    Event event = {"Ninguno", 0, 0, false, 1};

    void LoadUpgrades();
    void LoadAchievements();
    void LoadMissions();
    void CheckAchievements();
    void CheckMissions();
    void CheckLevelUp();
    void UnlockAchievement(int index);
    void SpawnRandomEvent();
    void AddXP(int amount);
    
    void DrawClickArea();
    void DrawUI();
    void DrawUpgradePanel();
    void DrawParticles();
    void DrawAchievements();
    void DrawMissions();
    void DrawCombo();
    void DrawEvent();
    void DrawLevel();
    void DrawStats();
    
    void SpawnClickParticles(Vector2 pos, int count);
    void SpawnFloatText(Vector2 pos, const char* text, Color color);
    void UpdateParticles();
};