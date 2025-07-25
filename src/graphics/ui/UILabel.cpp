#include "graphics/ui/UILabel.hpp"
#include <iostream>

using namespace std;

UILabel::UILabel(Vector2 pos, const string &txt, int sz,
                 float dur, Color fontClr, Color cadreClr) : position(pos),
                                                             text(txt),
                                                             fontSize(sz),
                                                             fontColor(fontClr),
                                                             cadreColor(cadreClr),
                                                             duration(dur),
                                                             timer(0.0f)
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

    float bgWidth = textSize.x + 2 * padding;
    float bgHeight = textSize.y + 2 * padding;

    if (drawBackground)
    {
        DrawRectangleRounded({position.x, position.y, bgWidth, bgHeight}, 0.2f, 20, backgroundColor);
        DrawRectangleRoundedLinesEx({position.x, position.y, bgWidth, bgHeight}, 0.2f, 20, 2.0f, cadreColor);
    }

    Vector2 drawPos;
    drawPos.x = position.x + (bgWidth - textSize.x) / 2.0f;
    drawPos.y = position.y + (bgHeight - textSize.y) / 2.0f;

    if (useCustomFont)
    {
        DrawTextEx(font, text.c_str(), drawPos, (float)fontSize, 2, fontColor);
    }
    else
    {
        DrawText(text.c_str(), drawPos.x, drawPos.y, fontSize, fontColor);
    }
}