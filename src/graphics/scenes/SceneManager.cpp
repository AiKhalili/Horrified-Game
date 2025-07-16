#include "graphics/scenes/SceneManager.hpp"
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

void SceneManager::goTo(const std::string &name)
{
    auto it = scenes.find(name);
    if (it != scenes.end())
    { // if the scene was registered befor , change the scene
        currentScene = it->second.get();
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