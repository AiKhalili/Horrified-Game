#include "graphics/ui/UIButton.hpp"

UIButton::UIButton(Rectangle bds, const std::string &txt,
                   int sz, Color txtColor,
                   Color bg, Color hover, Color cdr)
    : bounds(bds), text(txt), fontSize(sz),
      textColor(txtColor), bgColor(bg), hoverColor(hover), cadreColor(cdr) {}

void UIButton::setOnClick(std::function<void()> callback)
{ // تابع روی کلید به متغیر onClick انتساب داده میشه
    onClick = callback;
}

void UIButton::setFocus(bool f)
{
    isFocused = f;
}

void UIButton::setFont(Font f)
{
    font = f;
    useCustomFont = true;
}

void UIButton::click()
{
    if (onClick)
    {
        onClick();
    }
}

void UIButton::update()
{
    Vector2 mousePos = GetMousePosition();
    isHovered = CheckCollisionPointRec(mousePos, bounds);

    if (isHovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    { // اگه موس روی دکمه بود و کاربر چپ کلیک کرد
        if (onClick)
        {
            onClick();
        }
    }
}

void UIButton::render()
{
    Color currentColor = isHovered ? hoverColor : bgColor;

    DrawRectangleRounded(bounds, 0.2f, 20, currentColor); // رسم مستطیل پس زمینه

    if (isFocused)
    { // کشیدن کادر رنگی دور مستطیل
        DrawRectangleRoundedLinesEx(bounds, 0.2f, 20, 2.0f, cadreColor);
    }

    Vector2 textSize; // اندازی گیری سایز متن برای اینکه دقیقا وسط کادر باشه
    if (useCustomFont)
    {
        textSize = MeasureTextEx(font, text.c_str(), (float)fontSize, 2);
    }
    else
    {
        int w = MeasureText(text.c_str(), fontSize); // ارتفاع
        textSize = Vector2{(float)w, (float)fontSize};
    }

    // محاسبه محل رسم متن
    Vector2 textPos = {bounds.x + (bounds.width - textSize.x) / 2,
                       bounds.y + (bounds.height - textSize.y) / 2};

    // رسم متن
    if (useCustomFont)
    {
        DrawTextEx(font, text.c_str(), textPos, (float)fontSize, 2, textColor);
    }
    else
    {
        DrawText(text.c_str(), (int)textPos.x, (int)textPos.y, fontSize, textColor);
    }
}