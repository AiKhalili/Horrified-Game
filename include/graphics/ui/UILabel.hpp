#ifndef UILABLE_HPP
#define UILABLE_HPP

#include "graphics/ui/UIElement.hpp"
#include <string>
#include "raylib.h"

class UILabel : public UIElement
{
private:
    Vector2 position;           // موقیت متن
    std::string text;           // خود متنی که قراره نمایش داده بشه
    int fontSize;               // اندازه فونت
    Color color;                // رنگ متن
    Font font;                  // فونت متن
    bool useCustomFont = false; // آیا از فونت سفارشی استفاده بشه یا نه؟
    float duration = 0.0f;      // چند ثانیه نمایش داده بشه
    float timer = 0.0f;         // شمارنده زمان سپری شده

public:
    UILabel(Vector2 pos, const std::string &text, int fontSize = 20, Color color = BLACK, float duration = 0.0f);
    void render() override;
    void update() override;
    void setFont(Font);
    bool isExoired() const;
};

#endif