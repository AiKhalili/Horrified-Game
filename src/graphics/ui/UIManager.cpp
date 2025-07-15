#include "graphics/ui/UIManager.hpp"

void UIManager::add(std::unique_ptr<UIElement> element)
{
    if (auto *btn = dynamic_cast<UIButton *>(element.get()))
    {
        buttons.push_back(btn);
        if (selectedIndex == -1)
        {
            selectedIndex = 0; // choose the first button
            btn->setFocus(true);
        }
    }
    elements.push_back(std::move(element));
}

void UIManager::update()
{
    if (!buttons.empty())
    { // only when there is at least one button
        if (IsKeyPressed(KEY_DOWN))
        { // the down key = going to the next one
            buttons[selectedIndex]->setFocus(false);
            selectedIndex = (selectedIndex + 1) % buttons.size();
            buttons[selectedIndex]->setFocus(true);
        }
        if (IsKeyPressed(KEY_UP))
        { // the up key = going to the previous key
            buttons[selectedIndex]->setFocus(false);
            selectedIndex = (selectedIndex - 1 + buttons.size()) % buttons.size();
            buttons[selectedIndex]->setFocus(true);
        }
        if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE))
        { // when Enter or Space is pressed, perform a click action to run the button's function
            buttons[selectedIndex]->click();
        }
    }
    for (auto &e : elements)
    { // update all of the elements
        e->update();
    }
}

void UIManager::render()
{ // render all of the elements
    for (auto &e : elements)
    {
        e->render();
    }
}

void UIManager::clear()
{ // clear all widgets and reset focus state
    elements.clear();
    buttons.clear();
    selectedIndex = -1;
}