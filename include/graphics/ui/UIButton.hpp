#ifndef UIBUTTON_HPP
#define UIBUTTON_HPP

#include "graphics/ui/UIElement.hpp"
#include <string>
#include <functional>
#include "raylib.h"

class UIButton : public UIElement
{
private:
    Rectangle bounds; // مکان و اندازه دکمه
    std::string text; // متن
    int fontSize;     // اندازه فونت متن
    Font font;        // فونت متن

    Color textColor;  // رنگ متن درون دکمه
    Color bgColor;    // رنگ دکمه در حالت عادی
    Color hoverColor; // رنگ دکمه وفتی کلیک شده روش

    bool isHovered = false;     // آیا موس روی دکمه کیبورد قرار داره یا نه؟
    bool isFocused = false;     // آیا دکمه با کیبورد انتخاب شده؟
    bool useCustomFont = false; // آیا از فونت سفارشی استفاده بشه؟

    std::function<void()> onClick; // تابعی که وقتی کلیک شد رو دکمه اجرا بشه

public:
    UIButton(Rectangle bounds, const std::string &text,
             int fontSize = 20, Color textColor = BLACK,
             Color bgColor = GRAY, Color hoverColor = DARKGRAY);

    void render() override; // رسم دکمه
    void update() override; // بررسی وضعیت موس و کلیک
    void click();           // اجرای تابع کلیک از طریق کیبورد

    // توابع set
    void setFont(Font);
    void setOnClick(std::function<void()>); // تنظیم تابع برای کلیک کردن
    void setFocus(bool);                    // کلیک روی دکمه از طریق کیبورد
};

#endif