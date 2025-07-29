#ifndef SCENEMANAGER_HPP
#define SCENEMANAGER_HPP

#include <unordered_map>
#include <memory>
#include <string>
#include "graphics/scenes/Scene.hpp"
#include <stdexcept>

class SceneManager
{
public:
    static SceneManager &getInstance();
    void registerScene(const std::string &name, std::unique_ptr<Scene> scene); // register the scene with a unique name
    void goTo(const std::string &name);                                        // change the scene
    void registerAllScenes();
    void requestExit();
    bool isExitRequested() const;

    // call in *main*
    void update(float deleteTime);
    void render();

    template <typename T>
    T &getScene(const std::string &name)
    {
        auto it = scenes.find(name);
        if (it == scenes.end())
        {
            throw std::runtime_error("Scene <" + name + "> not found!");
        }
        T *scene = dynamic_cast<T *>(it->second.get());
        if (!scene)
        {
            throw std::runtime_error("Scene <" + name + "> is not of requested type!");
        }
        return *scene;
    }

private:
    SceneManager() = default;
    SceneManager(const SceneManager &) = delete;
    SceneManager &operator=(const SceneManager &) = delete;
    std::unordered_map<std::string, std::unique_ptr<Scene>> scenes; // all of the scenes
    Scene *currentScene = nullptr;                                  // the current scene
    bool exitRequest = false;
};

#endif