#include "graphics/scenes/ItemSelectionScene.hpp"
#include "graphics/scenes/SceneManager.hpp"
#include "graphics/scenes/SceneKeys.hpp"
#include "graphics/ui/UIButton.hpp"
#include "graphics/ui/UILabel.hpp"
#include "graphics/TextureManager.hpp"
#include "audio/AudioManager.hpp"
#include "saves/SaveManager.hpp"
#include <memory>
#include <vector>
#include <iostream>
#include <cmath>
#include "core/Game.hpp"

void ItemSelectionScene::setData(const std::vector<Item *> &Items)
{
    items = Items;
    // hero = Game::getInstance().getCurrentHero();
    location = Items[0]->get_location();
}

void ItemSelectionScene::onEnter()
{
    background = TextureManager::getInstance().getOrLoadTexture(
        "ShowActions", "assets/images/background/item_selection.png");
    font = LoadFont("assets/fonts/simple.ttf");

    createLabels();

    std::sort(items.begin(), items.end(), [](Item *a, Item *b)
              {
        auto colorOrder = [](COlOR c) {
            switch(c) {
                case COlOR::red: return 0;
                case COlOR::yellow: return 1;
                case COlOR::blue: return 2;
                default: return 3;
            }
        };
        return colorOrder(a->get_color()) < colorOrder(b->get_color()); });

    selected.clear();
    itemTextures.clear();
    itemRects.clear();

    loadItemTextures();

    int maxItems = (int)std::min(items.size(), size_t(32));
    int cols = 8;
    int rows = 4;
    float spacing = 10.0f;

    float totalWidth = 1600.0f;
    float itemSize = (totalWidth - (cols - 1) * spacing) / cols * 0.8f;

    float totalItemsWidth = cols * itemSize + (cols - 1) * spacing;
    float startX = (1600.0f - totalItemsWidth) / 2.0f;
    float startY = 120.0f;

    for (int i = 0; i < maxItems; ++i)
    {
        int row = i / cols;
        int col = i % cols;
        Rectangle rect = {
            startX + col * (itemSize + spacing),
            startY + row * (itemSize + spacing),
            itemSize,
            itemSize};
        itemRects.push_back(rect);
    }

    SetTextureFilter(font.texture, TEXTURE_FILTER_BILINEAR);
}

void ItemSelectionScene::onExit()
{
    UnloadFont(font);
    ui.clear();
    selected.clear();
    items.clear();
    itemRects.clear();
}

void ItemSelectionScene::update(float deltaTime)
{
    AudioManager::getInstance().update();
    Vector2 mousePos = GetMousePosition();

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        for (size_t i = 0; i < itemRects.size(); ++i)
        {
            if (CheckCollisionPointRec(mousePos, itemRects[i]))
            {
                toggleSelection(items[i]);
                break;
            }
        }
    }

    confirmButtonFocused = CheckCollisionPointRec(mousePos, confirmButtonRect);
    ui.update();
}

void ItemSelectionScene::render()
{
    BeginDrawing();
    ClearBackground(BLACK);

    DrawTexturePro(background,
                   Rectangle{0, 0, (float)background.width, (float)background.height},
                   Rectangle{0, 0, 1600, 900},
                   Vector2{0, 0}, 0.0f, WHITE);

    Vector2 mousePos = GetMousePosition();

    for (size_t i = 0; i < itemTextures.size(); ++i)
    {
        DrawTexturePro(itemTextures[i],
                       Rectangle{0, 0, (float)itemTextures[i].width, (float)itemTextures[i].height},
                       itemRects[i],
                       Vector2{0, 0}, 0.0f, WHITE);

        if (std::find(selected.begin(), selected.end(), items[i]) != selected.end())
        {
            DrawRectangleLinesEx(itemRects[i], 5, RAYWHITE);
        }
        else if (CheckCollisionPointRec(mousePos, itemRects[i]))
        {
            DrawRectangleLinesEx(itemRects[i], 2, GRAY);
        }
    }

    ui.render();
    EndDrawing();
}

void ItemSelectionScene::createLabels()
{
    const char *text = "Please Select Item";
    int fontSize = 35;
    Vector2 textSize = MeasureTextEx(font, text, fontSize, 1);

    Color textcolor = {245, 230, 196, 255};
    Color labelcolor = {70, 50, 35, 255};
    Color clickcolor = {85, 65, 45, 255};

    auto selectText = std::make_unique<UILabel>(
        Vector2{(1600 - textSize.x) / 2.0f, 50}, text, fontSize, 0.0f, textcolor);
    selectText->setFont(font);
    ui.add(std::move(selectText));

    auto menuBtn = std::make_unique<UIButton>(Rectangle{1450, 840, 120, 40}, "Main Menu", 20, textcolor, labelcolor, clickcolor, textcolor);
    menuBtn->setFont(font);
    menuBtn->setOnClick([]()
                        {
        AudioManager::getInstance().playSoundEffect("click");
        SceneManager::getInstance().goTo(SceneKeys::MAIN_MENU_SCENE); });

    ui.add(std::move(menuBtn));

    auto saveBtn = std::make_unique<UIButton>(Rectangle{1450, 790, 120, 40}, "Save", 20, textcolor, labelcolor, clickcolor, textcolor);
    saveBtn->setFont(font);
    saveBtn->setOnClick([]()
                        {
        AudioManager::getInstance().playSoundEffect("click");
        SaveManager::getInstance().saveGameToSlot(); });

    ui.add(std::move(saveBtn));

    auto boardBtn = std::make_unique<UIButton>(Rectangle{1450, 740, 120, 40}, "Board Scene", 20, textcolor, labelcolor, clickcolor, textcolor);
    boardBtn->setFont(font);
    boardBtn->setOnClick([]()
                         {
        AudioManager::getInstance().playSoundEffect("click");
        SaveManager::getInstance().saveGameToSlot(); });

    ui.add(std::move(boardBtn));

    Color midCreamBrown ={140, 110, 70, 255};

    auto nonBtn = std::make_unique<UIButton>(Rectangle{570, 790, 130, 60}, "Non", 45, labelcolor, textcolor, clickcolor, midCreamBrown);
    nonBtn->setFont(font);
    nonBtn->setOnClick([]()
                       {
                           AudioManager::getInstance().playSoundEffect("click");
                       });

    ui.add(std::move(nonBtn));

    auto submtBtn = std::make_unique<UIButton>(Rectangle{900, 790, 130, 60}, "Submit", 45,
                                               labelcolor, textcolor, clickcolor, midCreamBrown);
    submtBtn->setFont(font);
    submtBtn->setOnClick([]()
                         { 
                             AudioManager::getInstance().playSoundEffect("click");
                         });
    ui.add(std::move(submtBtn));
}

void ItemSelectionScene::loadItemTextures()
{
    itemTextures.clear();
    for (auto *item : items)
    {
        std::string folder;
        switch (item->get_color())
        {
        case COlOR::red:
            folder = "assets/images/Items/Red/";
            break;
        case COlOR::blue:
            folder = "assets/images/Items/Blue/";
            break;
        case COlOR::yellow:
            folder = "assets/images/Items/Yellow/";
            break;
        default:
            folder = "assets/images/items/";
            break;
        }
        std::string path = folder + item->get_name() + ".png";
        Texture2D tex = TextureManager::getInstance().getOrLoadTexture(item->get_name(), path);
        itemTextures.push_back(tex);
    }
}

void ItemSelectionScene::toggleSelection(Item *item)
{
    auto it = std::find(selected.begin(), selected.end(), item);
    if (it == selected.end())
        selected.push_back(item);
    else
        selected.erase(it);
}

std::vector<Item *> &ItemSelectionScene::getSelectedItems()
{
    return selected;
}