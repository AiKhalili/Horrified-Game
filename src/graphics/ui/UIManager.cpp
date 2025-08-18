#include "graphics/ui/UIManager.hpp"

void UIManager::add(std::unique_ptr<UIElement> element)
{
    if (auto *btn = dynamic_cast<UIButton *>(element.get()))
    {                           // check if the element if button
        buttons.push_back(btn); // add button to internal button list
        if (focusMode == FocusMode::None)
        {                      // if no element is focused yet, set focus to the fist button
            selectedIndex = 0; // choose the first button
            btn->setFocus(true);
            focusMode = FocusMode::Button;
        }
    }
    else if (auto *box = dynamic_cast<UIInputBox *>(element.get()))
    { // if no a button , check if it's an input box
        inputBoxes.push_back(box);
        if (focusMode == FocusMode::None)
        {
            selectedInputIndex = 0;
            box->setFocus(true);
            focusMode = FocusMode::Input;
        }
    }

    // store the element in the master list for rendering and updating
    elements.push_back(std::move(element));
}

void UIManager::update()
{
    if (IsKeyPressed(KEY_TAB))
    { // handle switching focus between buttons and inputBoxes using TAB
        if (focusMode == FocusMode::Button && !inputBoxes.empty())
        {
            buttons[selectedIndex]->setFocus(false);

            if (selectedInputIndex < 0 || selectedInputIndex >= inputBoxes.size())
            {
                selectedInputIndex = 0;
            }

            inputBoxes[selectedInputIndex]->setFocus(true);
            focusMode = FocusMode::Input;
        }
        else if (focusMode == FocusMode::Input && !buttons.empty())
        {
            inputBoxes[selectedInputIndex]->setFocus(false);

            if (selectedIndex < 0 || selectedIndex >= buttons.size())
            {
                selectedIndex = 0;
            }
            buttons[selectedIndex]->setFocus(true);
            focusMode = FocusMode::Button;
        }
    }

    if (focusMode == FocusMode::Input && !inputBoxes.empty())
    { // navigate between input boxes using arrow keys
        if (IsKeyPressed(KEY_DOWN))
        {
            inputBoxes[selectedInputIndex]->setFocus(false);
            selectedInputIndex = (selectedInputIndex + 1) % inputBoxes.size();
            inputBoxes[selectedInputIndex]->setFocus(true);
        }
        if (IsKeyPressed(KEY_UP))
        {
            inputBoxes[selectedInputIndex]->setFocus(false);
            selectedInputIndex = (selectedInputIndex - 1 + inputBoxes.size()) % inputBoxes.size();
            inputBoxes[selectedInputIndex]->setFocus(true);
        }
    }

    if (focusMode == FocusMode::Button && !buttons.empty())
    {
        if (IsKeyPressed(KEY_DOWN))
        {
            buttons[selectedIndex]->setFocus(false);
            selectedIndex = (selectedIndex + 1) % buttons.size();
            buttons[selectedIndex]->setFocus(true);
        }
        if (IsKeyPressed(KEY_UP))
        {
            buttons[selectedIndex]->setFocus(false);
            selectedIndex = (selectedIndex - 1 + buttons.size()) % buttons.size();
            buttons[selectedIndex]->setFocus(true);
        }
        if (IsKeyPressed(KEY_ENTER))
        {
            buttons[selectedIndex]->click();
        }
    }

    for (auto &e : elements)
    { // update all UI elements regardless of focus
        if (e)
        {
            e->update();
        }
    }
}

void UIManager::render()
{ // render all of the elements
    for (auto &e : elements)
    {
        if (e)
        {
            e->render();
        }
    }
}

void UIManager::clear()
{ // clear all widgets and reset focus state
    elements.clear();
    buttons.clear();
    inputBoxes.clear();
    selectedInputIndex = -1;
    selectedIndex = -1;
    focusMode = FocusMode::None;
}