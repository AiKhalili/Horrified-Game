#ifndef UILABLE_HPP
#define UILABLE_HPP

#include "graphics/ui/UIElement.hpp"
#include <string>
#include "raylib.h"

class UILabel : public UIElement
{
private:
    Vector2 position; // موقیت متن
    std::string text; // خود متنی که قراره نمایش داده بشه
    int fontSize;     // اندازه فونت
    Color fontColor;  // رنگ متن
    Color backgroundColor;
    Color cadreColor;
    Font font;                  // فونت متن
    bool useCustomFont = false; // آیا از فونت سفارشی استفاده بشه یا نه؟
    bool drawBackground = false;
    float duration;     // چند ثانیه نمایش داده بشه
    float timer = 0.0f; // شمارنده زمان سپری شده
    float padding;      // فاصله بین متن و کادر

public:
    UILabel(Vector2 pos, const std::string &text, int fontSize = 20, float duration = 0.0f,
            Color fontcolor = BLACK, Color cadreColor = RED);
    void render() override;
    void update() override;
    void setFont(const Font &);
    void enableBackground(Color bgColor, float pad = 10.0f);
    void setText(const std::string &newText);
    void resetTimer();
    std::string getText() { return text; }
};

#endif