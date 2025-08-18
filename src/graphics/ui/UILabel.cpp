#include "graphics/ui/UILabel.hpp"
#include <iostream>

using namespace std;

UILabel::UILabel(Vector2 pos, const string &txt, int sz,
                 float dur, Color fontClr, Color cadreClr, bool center) : position(pos),
                                                                          text(txt),
                                                                          fontSize(sz),
                                                                          fontColor(fontClr),
                                                                          cadreColor(cadreClr),
                                                                          duration(dur),
                                                                          timer(0.0f),
                                                                          centerAligment(center)
{
}

void UILabel::setFont(const Font &f)
{
    font = f;
    useCustomFont = true;
}

void UILabel::setText(const std::string &newText)
{
    text = newText;
    timer = 0.0f;
}

void UILabel::resetTimer()
{
    timer = 0.0f;
}

void UILabel::update()
{
    if (duration > 0.0f)
    {
        timer += GetFrameTime();
    }
}

void UILabel::enableBackground(Color bgColor, float pad)
{
    drawBackground = true;
    backgroundColor = bgColor;
    padding = pad;
}

void UILabel::render()
{
    if (duration > 0.0f && timer >= duration)
    {
        return;
    }

    Vector2 textSize;
    if (useCustomFont)
    {
        textSize = MeasureTextEx(font, text.c_str(), (float)fontSize, 2);
    }
    else
    {
        textSize = {(float)MeasureText(text.c_str(), fontSize), (float)fontSize};
    }

    Vector2 drawPos = position;

    if (centerAligment)
    {
        drawPos.x -= textSize.x / 2.0f;
        drawPos.y -= textSize.y / 2.0f;
    }

    if (drawBackground)
    {
        float bgWidth = textSize.x + 2 * padding;
        float bgHeight = textSize.y + 2 * padding;

        float bgX = position.x;
        float bgY = position.y;

        if (centerAligment)
        {
            bgX = position.x - bgWidth / 2.0f;
            bgY = position.y - bgHeight / 2.0f;
        }

        // زمینه و کادر
        DrawRectangleRounded({bgX, bgY, bgWidth, bgHeight}, 0.2f, 20, backgroundColor);
        DrawRectangleRoundedLinesEx({bgX, bgY, bgWidth, bgHeight}, 0.2f, 20, 2.0f, cadreColor);

        // وسط‌چینی متن درون کادر
        if (centerAligment)
        {
            drawPos.x = bgX + (bgWidth - textSize.x) / 2.0f;
            drawPos.y = bgY + (bgHeight - textSize.y) / 2.0f;
        }
        else
        {
            drawPos.x = bgX + padding;
            drawPos.y = bgY + padding;
        }
    }

    // رسم متن
    if (useCustomFont)
    {
        DrawTextEx(font, text.c_str(), drawPos, (float)fontSize, 2, fontColor);
    }
    else
    {
        DrawText(text.c_str(), drawPos.x, drawPos.y, fontSize, fontColor);
    }
}