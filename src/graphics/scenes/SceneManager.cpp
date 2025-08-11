#include "graphics/scenes/SceneManager.hpp"
#include "graphics/scenes/SceneManager.hpp"
#include "graphics/scenes/IntroScene.hpp"
#include "graphics/scenes/MainMenuScene.hpp"
#include "graphics/scenes/LoadGamesScene.hpp"
#include "graphics/scenes/BoardScene.hpp"
#include "graphics/scenes/SceneKeys.hpp"
#include "graphics/scenes/HeroSelectionScene.hpp"
#include "graphics/scenes/PlayerInfoScene.hpp"
#include "graphics/scenes/ShowActionsScene.hpp"
#include "graphics/scenes/ChestInfoScene.hpp"
#include "graphics/scenes/PerkCardScene.hpp"
#include "graphics/scenes/LocationInfoScene.hpp"
#include "graphics/scenes/ItemSelectionScene.hpp"
#include "graphics/scenes/MonsterPhaseScene.hpp"
#include "graphics/scenes/VillagerSelectionScene.hpp"
#include "graphics/scenes/LocationSelectionScene.hpp"
#include "graphics/scenes/PlayerSummaryScene.hpp"
#include "graphics/scenes/MoveScene.hpp"
#include "graphics/scenes/PickUpScene.hpp"
#include "graphics/scenes/RescueResultScene.hpp"
#include "graphics/scenes/EndGameScene.hpp"
#include "graphics/scenes/MonsterSelectionScene.hpp"
#include "graphics/scenes/GuidScene.hpp"
#include "graphics/scenes/AdvanceScene.hpp"
#include "graphics/scenes/DefeatScene.hpp"
#include "graphics/scenes/SpecialScene.hpp"
#include "graphics/scenes/HelpScene.hpp"
#include "graphics/scenes/PerkSelectionScene.hpp"
#include "graphics/scenes/MonstersSummaryScene.hpp"
#include <iostream>

SceneManager &SceneManager::getInstance()
{
    static SceneManager instance;
    return instance;
}

void SceneManager::registerScene(const std::string &name, std::unique_ptr<Scene> scene)
{
    scenes[name] = std::move(scene);
    if (!currentScene)
    { // set the first registered scene as the active one(only if no scene is active yet)
        currentScene = scenes[name].get();
    }
}

void SceneManager::registerAllScenes()
{
    registerScene(SceneKeys::INTRO_SCENE, std::make_unique<IntroScene>());
    registerScene(SceneKeys::MAIN_MENU_SCENE, std::make_unique<MainMenuScene>());
    registerScene(SceneKeys::LOAD_GAMES_SCENE, std::make_unique<LoadGamesScene>());
    registerScene(SceneKeys::BOARD_SCENE, std::make_unique<BoardScene>());
    registerScene(SceneKeys::PLAYER_INFO_SCENE, std::make_unique<PlayerInfoScene>());
    registerScene(SceneKeys::HERO_SELECTION_SCENE, std::make_unique<HeroSelectionScene>());
    registerScene(SceneKeys::SHOW_ACTIONS_SCENE, std::make_unique<ShowActionsScene>());
    registerScene(SceneKeys::CHEST_INFO_SCENE, std::make_unique<ChestInfoScene>());
    registerScene(SceneKeys::PERK_CARD_SCENE, std::make_unique<PerkCardScene>());
    registerScene(SceneKeys::LOCATION_INFO_SCENE, std::make_unique<LocationInfoScene>());
    registerScene(SceneKeys::ITEM_SELECTION_SCENE, std::make_unique<ItemSelectionScene>());
    registerScene(SceneKeys::MONSTER_PHASE_SCENE, std::make_unique<MonsterPhaseScene>());
    registerScene(SceneKeys::VILLAGER_SELECTION_SCENE, std::make_unique<VillagerSelectionScene>());
    registerScene(SceneKeys::LOCATION_SELECTION_SCENE, std::make_unique<LocationSelectionScene>());
    registerScene(SceneKeys::MOVE_SCENE, std::make_unique<MoveScene>());
    registerScene(SceneKeys::PICK_UP_SCENE, std::make_unique<PickUpScene>());
    registerScene(SceneKeys::PLAYER_SUMMARY_SCENE, std::make_unique<PlayerSummaryScene>());
    registerScene(SceneKeys::RESCUE_RESULT_SCENE, std::make_unique<RescueResultScene>());
    registerScene(SceneKeys::END_GAME_SCENE, std::make_unique<EndGameScene>());
    registerScene(SceneKeys::MONSTER_SELECTION_SCENE, std::make_unique<MonsterSelectionScene>());
    registerScene(SceneKeys::GUID_SCENE, std::make_unique<GuidScene>());
    registerScene(SceneKeys::ADVANCE_SCENE, std::make_unique<AdvanceScene>());
    registerScene(SceneKeys::DEFEAT_SCENE, std::make_unique<DefeatScene>());
    registerScene(SceneKeys::SPECIAL_SCENE, std::make_unique<SpecialScene>());
    registerScene(SceneKeys::HELP_SCENE, std::make_unique<HelpScene>());
    registerScene(SceneKeys::PERK_SELECTION_SCENE, std::make_unique<PerkSelectionScene>());
    registerScene(SceneKeys::MONSTERS_SUMMARY_SCENE, std::make_unique<MonstersSummaryScene>());
}

void SceneManager::goTo(const std::string &name)
{
    auto it = scenes.find(name);
    if (it != scenes.end())
    { // if the scene was registered befor , change the scene
        if (currentScene)
        {
            if (!currentScene->getSkip())
            {
                currentScene->onExit(); // before change the scene
            }
        }
        currentScene = it->second.get();
        if (currentScene)
        {
            if (!currentScene->getSkip())
            {
                currentScene->onEnter(); // after change the scene
            }
            currentScene->setSkip(false);
        }
        else
        {
            std::cerr << "Scene <" << name << "> not found!\n";
        }
    }
}

void SceneManager::update(float deleteTime)
{
    if (currentScene)
    {
        currentScene->update(deleteTime);
    }
}

void SceneManager::render()
{
    if (currentScene)
    {
        currentScene->render();
    }
}

void SceneManager::requestExit()
{
    exitRequest = true;
}

bool SceneManager::isExitRequested() const
{
    return exitRequest;
}