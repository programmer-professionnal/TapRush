#pragma once
#include <raylib.h>

class Button {
public:
    Button() : x(0), y(0), width(100), height(40) {}
    Button(float x, float y, float width, float height, const char* text);

    void Draw(Color bgColor, Color textColor);
    bool IsClicked();
    bool IsHovered() const;

    Rectangle GetRect() const { return {x, y, width, height}; }
    float GetX() const { return x; }
    float GetY() const { return y; }

private:
    float x, y, width, height;
    char text[32];
};