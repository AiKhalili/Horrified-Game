#include "graphics/scenes/ChestInfoScene.hpp"
#include "core/Game.hpp"
#include "graphics/scenes/BoardScene.hpp"
#include "graphics/scenes/SceneManager.hpp"
#include "graphics/scenes/SceneKeys.hpp"
#include "audio/AudioManager.hpp"

void ChestInfoScene::onEnter()
{
    currentHero = Game::getInstance().getCurrentHero();
    background = LoadTexture("assets/images/background/wooden.png");
    font = LoadFont("assets/fonts/simple.ttf");

    AudioManager::getInstance().playMusic();

    uiButton = std::make_unique<UIButton>(Rectangle{600, 650, 200, 50}, "Back", 28, WHITE, DARKBROWN, BROWN);
    uiButton->setOnClick([this]()
                         { 
                            AudioManager::getInstance().playSoundEffect("click");
                            SceneManager::getInstance().goTo(SceneKeys::BOARD_SCENE); });

    uiManager.add(std::move(uiButton));
    loadHeroItems();
}