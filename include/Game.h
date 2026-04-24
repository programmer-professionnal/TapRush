#pragma once
#include <raylib.h>
#include <vector>
#include <string>
#include "Button.h"

enum MenuTab {
    TAB_UPGRADES,
    TAB_MISSIONS,
    TAB_PROGRESS,
    TAB_STATS
};

struct Upgrade {
    std::string name;
    long long cost;
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

struct WeeklyMission {
    std::string name;
    std::string description;
    int target;
    int current;
    bool completed;
    int reward;
    bool claimed;
};

struct LeaderboardEntry {
    std::string name;
    int score;
    int level;
    int prestige;
};

class Game {
public:
    void Init();
    void Update();
    void Draw();
    void Save();
    void Load();
    void SetTab(MenuTab tab) { currentTab = tab; }

    void AddUpgrade(const std::string& name, int baseCost, int cps);
    void BuyUpgrade(int index);
    void Click();

    int GetScore() const { return (int)score; }
    long long GetDisplayScore() const { return score * prestigeMultiplier; }
    int GetClicksPerSecond() const;
    int GetClickValue() const {
        int base = 1 + combo;
        for (const auto& up : upgrades) {
            if (up.clicksPerSecond < 0) base += up.count;
        }
        if (event.active) base *= event.multiplier;
        return base * prestigeMultiplier * streakMultiplier;
    }
    int GetCursorCount() const;
    int GetLevel() const { return level; }
    float GetXP() const { return xp; }
    float GetCombo() const { return combo; }
    MenuTab GetTab() const { return currentTab; }

void AddStreak() { if (dailyStreak < 0) dailyStreak = 0; dailyStreak++; lastPlayDay = GetDayOfYear(); UpdateStreakMultiplier(); }
    void UseEnergy() { if (energy > 0) energy--; }
    int GetEnergy() const { return energy; }
    int GetMaxEnergy() const { return maxEnergy; }
    void RegenerateEnergy() { if (energy < maxEnergy) energy++; }
    bool CanClick() const { return energy > 0 || energyUnlimited; }
    void SetEnergyUnlimited(bool u) { energyUnlimited = u; }
    void AddToLeaderboard(int s, int l, int p) {
        LeaderboardEntry entry;
        entry.name = "Tú";
        entry.score = s;
        entry.level = l;
        entry.prestige = p;
        leaderboard.push_back(entry);
        for (size_t i = 0; i < leaderboard.size() - 1; i++) {
            for (size_t j = i + 1; j < leaderboard.size(); j++) {
                if (leaderboard[j].score > leaderboard[i].score) {
                    LeaderboardEntry temp = leaderboard[i];
                    leaderboard[i] = leaderboard[j];
                    leaderboard[j] = temp;
                }
            }
        }
        if (leaderboard.size() > 10) leaderboard.resize(10);
    }
    const std::vector<LeaderboardEntry>& GetLeaderboard() const { return leaderboard; }
    int GetDayOfYear();
    void AddXP(int amount);
    void UpdateStreakMultiplier();
    
    void CheckDailyReset();
    void CheckWeeklyReset();
    void AddWeeklyProgress(const std::string& type, int amount);
    void Prestige();
    
    void DrawClickArea();
    void DrawUI();
    void DrawMenuTabs();
    void DrawUpgradePanel();
    void DrawMissionsPanel();
    void DrawProgressPanel();
    void DrawStatsPanel();
    void DrawParticles();
    void DrawAchievements();
    void DrawCombo();
    void DrawEvent();
    void DrawLevel();
    void DrawStats();
    void DrawEnergy();
    void DrawStreak();
    void DrawPrestige();
    void DrawSpecialEvent();
    void DrawLeaderboard();
    void LoadWeeklyMissions();
    void CheckWeeklyMissions();
    void LoadUpgrades();
    void LoadAchievements();
    void LoadMissions();
    void CheckAchievements();
    void CheckMissions();
    void CheckLevelUp();
    void UnlockAchievement(int index);
    void SpawnRandomEvent();
    void SpawnSpecialEvent();
    
    void SpawnClickParticles(Vector2 pos, int count);
    void SpawnFloatText(Vector2 pos, const char* text, Color color);
    void UpdateParticles();

private:
    long long score = 0;
    float scoreFloat = 0;
    long long totalClicks = 0;
    int level = 1;
    float xp = 0;
    int combo = 0;
    float comboTimer = 0;
    MenuTab currentTab = TAB_UPGRADES;
    std::vector<Upgrade> upgrades;
    std::vector<Button> upgradeButtons;
    std::vector<Button> menuTabs;
    std::vector<Particle> particles;
    std::vector<Particle> floatTexts;
    std::vector<Achievement> achievements;
    std::vector<Mission> missions;
    std::vector<Mission> dailyMissions;

    int energy = 500;
    int maxEnergy = 500;
    float energyRegenTimer = 0;
    bool energyUnlimited = false;
    int dailyStreak = 0;
    int lastPlayDay = 0;
    int streakMultiplier = 1;
    int prestigeLevel = 0;
    int prestigeMultiplier = 1;

    int weeklyScore = 0;
    int weeklyClicks = 0;
    int weeklyUpgrades = 0;
    int weeklyStartDay = 0;
    std::vector<WeeklyMission> weeklyMissions;

    std::vector<LeaderboardEntry> leaderboard;
    float screenShake = 0;
    float levelUpAnim = 0;

    Button clickArea;
    float clickAnimTimer = 0;
    float starRotation = 0;
    float pulseTimer = 0;
    float autoClickTimer = 0;
    float achievementTimer = 0;
    float eventTimer = 0;
    float specialEventTimer = 0;
    std::string specialEventName = "";
    bool specialEventActive = false;
    
    Event event = {"Ninguno", 0, 0, false, 1};
};