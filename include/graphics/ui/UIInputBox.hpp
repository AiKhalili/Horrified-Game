#ifndef UIINPUTBOX_HPP
#define UIINPUTBOX_HPP

#include "graphics/ui/UIElement.hpp"
#include <string>
#include "raylib.h"

class UIInputBox : public UIElement
{
private:
    Rectangle bounds;
    std::string text; // user-enterd text
    bool isFocused = false;
    int maxLength;
    float blinkTime = 0.0f;
    bool showCursor = true;
    Color boxColor;
    Color cadreColor;
    Color textColor;
    Font font;
    bool useCustomFont = false;
    int fontSize = 20;

public:
    UIInputBox(Rectangle bounds, int fontSize = 24, int maxLength = 20,
               Color boxColor = DARKGRAY, Color cadreColor = RED, Color textColor = BLACK);
    void update() override;
    void render() override;

    std::string getText() const;
    
    void setText(const std::string&);
    void setFocus(bool);
    void setFont(Font);
    void setMaxLength(int);
};

#endif