#ifndef UIMANAGER_HPP
#define UIMANAGER_HPP

#include <vector>
#include <memory>
#include "graphics/ui/UIElement.hpp"
#include "graphics/ui/UIButton.hpp"
#include "raylib.h"

class UIManager
{
public:
    void add(std::unique_ptr<UIElement> element);
    void update();
    void render();
    void clear();

private:
    std::vector<std::unique_ptr<UIElement>> elements; // all of the elements : buttons and labels
    std::vector<UIButton *> buttons; // just buttons
    int selectedIndex = -1; // By default, no button is selected(-1)
};

#endif