#include "graphics/ui/UILabel.hpp"
#include <iostream>

using namespace std;

UILabel::UILabel(Vector2 pos, const string &txt, int sz, Color clr, float dur) : position(pos),
                                                                                 text(txt),
                                                                                 fontSize(sz),
                                                                                 color(clr),
                                                                                 duration(dur) {}

void UILabel::setFont(Font f)
{
    font = f;
    useCustomFont = true;
}

void UILabel::update()
{
    if (duration > 0.0f)
    {
        timer += GetFrameTime();
    }
}

void UILabel::render()
{
    if (duration > 0.0f && timer >= duration)
    {
        return;
    }
    if (useCustomFont)
    {
        DrawTextEx(font, text.c_str(), position, (float)fontSize, 2, color);
    }
    else
    {
        DrawText(text.c_str(), position.x, position.y, fontSize, color);
    }
}

bool UILabel::isExoired() const
{
    return (duration > 0.0f && timer >= duration);
}