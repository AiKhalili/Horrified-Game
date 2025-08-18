#ifndef UIMANAGER_HPP
#define UIMANAGER_HPP

#include <vector>
#include <memory>
#include "graphics/ui/UIElement.hpp"
#include "graphics/ui/UIButton.hpp"
#include "graphics/ui/UIInputBox.hpp"
#include "raylib.h"

enum class FocusMode
{
    None,
    Button,
    Input
};

class UIManager
{
public:
    void add(std::unique_ptr<UIElement> element);
    void update();
    void render();
    void clear();

private:
    std::vector<std::unique_ptr<UIElement>> elements; // all of the elements : buttons and labels
    std::vector<UIButton *> buttons;                  // just buttons
    std::vector<UIInputBox *> inputBoxes;             // just input boxes
    int selectedIndex = -1;                            // By default, no button is selected(-1)
    int selectedInputIndex = -1;                      // By default, no input is selected(-1)
    FocusMode focusMode = FocusMode::None;
};

#endif