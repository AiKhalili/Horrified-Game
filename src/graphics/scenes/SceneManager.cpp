#include "graphics/scenes/SceneManager.hpp"
#include "graphics/scenes/SceneManager.hpp"
#include "graphics/scenes/IntroScene.hpp"
#include "graphics/scenes/MainMenuScene.hpp"
#include "graphics/scenes/LoadGamesScene.hpp"
#include "graphics/scenes/BoardScene.hpp"
#include "graphics/scenes/SceneKeys.hpp"
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
}

void SceneManager::goTo(const std::string &name)
{
    auto it = scenes.find(name);
    if (it != scenes.end())
    { // if the scene was registered befor , change the scene
        if (currentScene)
        {
            currentScene->onExit(); // before change the scene
        }
        currentScene = it->second.get();
        if (currentScene)
        {
            currentScene->onEnter(); // after change the scene
        }
    }
    else
    {
        std::cerr << "Scene <" << name << "> not found!\n";
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