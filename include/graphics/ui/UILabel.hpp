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
    Color color;      // رنگ متن
    Color backgroundColor;
    Color cadreColor;
    Font font;                  // فونت متن
    bool useCustomFont = false; // آیا از فونت سفارشی استفاده بشه یا نه؟
    bool drawBackground = false;
    float duration;     // چند ثانیه نمایش داده بشه
    float timer = 0.0f; // شمارنده زمان سپری شده
    float padding;      // فاصله بین متن و کادر

public:
    UILabel(Vector2 pos, const std::string &text, int fontSize = 20,
            Color color = BLACK, Color cadreColor = RED, float duration = 0.0f);
    void render() override;
    void update() override;
    void setFont(Font);
    bool isExoired() const;
    void enableBackground(Color bgColor, float pad = 10.0f);
};

#endif