#include "Button.h"
#include <raylib.h>
#include <cstring>

Button::Button(float x, float y, float width, float height, const char* text) 
    : x(x), y(y), width(width), height(height) {
    strncpy(this->text, text, 31);
    this->text[31] = '\0';
}

void Button::Draw(Color bgColor, Color textColor) {
    bool hovered = IsHovered();
    Color borderColor = hovered ? GOLD : Color{100, 100, 120, 255};
    
    DrawRectangle(x, y, width, height, bgColor);
    DrawRectangleLines(x, y, width, height, borderColor);
    
    if (text[0] != '\0') {
        float textWidth = MeasureText(text, 20);
        DrawText(text, x + (width - textWidth) / 2, y + (height - 20) / 2, 20, textColor);
    }
}

bool Button::IsClicked() {
    return IsHovered() && IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
}

bool Button::IsHovered() const {
    Vector2 mouse = GetMousePosition();
    return mouse.x >= x && mouse.x <= x + width && 
           mouse.y >= y && mouse.y <= y + height;
}