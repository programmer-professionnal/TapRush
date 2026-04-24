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
};

class Game {
public:
    void Init();
    void Update();
    void Draw();

    void AddUpgrade(const std::string& name, int baseCost, int cps);
    void BuyUpgrade(int index);
    void Click();

    int GetScore() const { return score; }
    int GetClicksPerSecond() const;
    int GetClickValue() const { return 1 + clickMultiplier; }

private:
    int score = 0;
    int totalClicks = 0;
    int clickMultiplier = 0;
    int autoClickCps = 0;
    std::vector<Upgrade> upgrades;
    std::vector<Button> upgradeButtons;

    Button clickArea;
    bool isClicking = false;
    float clickAnimTimer = 0;

    void LoadUpgrades();
    void DrawClickArea();
    void DrawUI();
    void DrawUpgradePanel();
};