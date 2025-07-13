#include "graphics/ui/UILabel.hpp"
#include <iostream>

using namespace std;

UILabel::UILabel(Vector2 pos, const string &txt, int sz, Color clr) : position(pos),
                                                                      text(txt),
                                                                      fontSize(sz),
                                                                      color(clr) {}

void UILabel::setFont(Font f)
{
    font = f;
    useCustomFont = true;
}

void UILabel::render()
{
    if (useCustomFont)
    {
        DrawTextEx(font, text.c_str(), position, (float)fontSize, 2, color);
    }
    else
    {
        DrawText(text.c_str(), position.x, position.y, fontSize, color);
    }
}