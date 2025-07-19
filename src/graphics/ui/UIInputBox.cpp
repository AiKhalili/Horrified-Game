#include "graphics/ui/UIInputBox.hpp"

UIInputBox::UIInputBox(Rectangle bounds, int fontSize, int maxLength,
                       Color boxColor, Color cadreColor, Color textColor)
    : bounds(bounds), fontSize(fontSize), maxLength(maxLength),
      boxColor(boxColor), cadreColor(cadreColor), textColor(textColor) {}

void UIInputBox::setFont(Font f)
{
    font = f;
    useCustomFont = true;
}

void UIInputBox::setFocus(bool focus)
{
    isFocused = focus;
}

void UIInputBox::setText(const std::string &newText)
{
    text = newText;
}

std::string UIInputBox::getText() const
{
    return text;
}

void UIInputBox::update()
{
    if (isFocused)
    {
        int key = GetCharPressed();
        while (key > 0)
        {
            if (text.length() < (size_t)maxLength)
            {
                text += static_cast<char>(key);
            }
            key = GetCharPressed();
        }

        if (IsKeyPressed(KEY_BACKSPACE) && !text.empty())
        { // delete with the backspace
            text.pop_back();
            backspaceTimer = 0.0f; // start the timer
            backspaceHeld = true;
        }
        else if (IsKeyDown(KEY_BACKSPACE) && !text.empty())
        {
            backspaceTimer += GetFrameTime();
            if (backspaceHeld && backspaceTimer > backspaceDelay)
            {
                text.pop_back();
                backspaceTimer = 0.0f;
            }
        }
        else
        {
            backspaceHeld = false;
        }

        blinkTime += GetFrameTime();
        if (blinkTime >= 0.5f)
        {
            showCursor = !showCursor;
            blinkTime = 0.0f;
        }
    }

    if (CheckCollisionPointRec(GetMousePosition(), bounds))
    { // check focus with the mouse
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            isFocused = true;
        }
    }
    else if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        isFocused = false;
    }
}

void UIInputBox::render()
{
    DrawRectangleRec(bounds, boxColor);
    DrawRectangleLinesEx(bounds, 2, cadreColor);

    std::string displayText = text;
    if (isFocused && showCursor && text.length() < (size_t)maxLength)
    {
        displayText += "|";
    }

    Vector2 textPos = {bounds.x + 5, bounds.y + (bounds.height - fontSize) / 2.0f};

    if (useCustomFont)
    {
        DrawTextEx(font, displayText.c_str(), textPos, (float)fontSize, 2, textColor);
    }
    else
    {
        DrawText(displayText.c_str(), (int)textPos.x, (int)textPos.y, fontSize, textColor);
    }
}

void UIInputBox::setMaxLength(int len)
{
    maxLength = len;
}