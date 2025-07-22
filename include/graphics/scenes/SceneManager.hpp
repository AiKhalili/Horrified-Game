#ifndef SCENEMANAGER_HPP
#define SCENEMANAGER_HPP

#include <unordered_map>
#include <memory>
#include <string>
#include "graphics/scenes/Scene.hpp"

class SceneManager
{
public:
    static SceneManager &getInstance();
    void registerScene(const std::string &name, std::unique_ptr<Scene> scene); // register the scene with a unique name
    void goTo(const std::string &name);                                        // change the scene
    void registerAllScenes();

    // call in *main*
    void update(float deleteTime);
    void render();

private:
    SceneManager() = default;
    SceneManager(const SceneManager &) = delete;
    SceneManager &operator=(const SceneManager &) = delete;
    std::unordered_map<std::string, std::unique_ptr<Scene>> scenes; // all of the scenes
    Scene *currentScene = nullptr;                                  // the current scene
};

#endif