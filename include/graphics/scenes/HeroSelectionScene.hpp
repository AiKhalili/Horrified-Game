#ifndef HEROSELECTIONSCENE_HPP
#define HEROSELECTIONSCENE_HPP

#include "graphics/scenes/Scene.hpp"
#include "graphics/ui/UIManager.hpp"
#include "graphics/ui/UIButton.hpp"
#include "graphics/ui/UILabel.hpp"
#include "raylib.h"
#include <memory>
#include <vector>

class HeroSelectionScene : public Scene
{
public:
    HeroSelectionScene();
    ~HeroSelectionScene() override;

    void update(float deltaTime) override;
    void render() override;

private:
    UIManager uiManager;

    Texture2D background;
    Font font;

    static constexpr int HERO_COUNT = 4;
    Texture2D heroTextures[HERO_COUNT];
    std::vector<std::unique_ptr<UIButton>> heroButtons;

    std::unique_ptr<UILabel> titleLabel;

    Rectangle heroButtonRects[HERO_COUNT];

    int currentPlayer = 1;
    int playerSelections[2] = { -1, -1 };

    void onHeroSelected(int heroIndex);
    bool isButtonEnabled(int heroIndex) const;
};

#endif
