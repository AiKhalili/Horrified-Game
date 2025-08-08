#include "app/GameApp.hpp"
#include "raylib.h"
#include "audio/AudioManager.hpp"
#include "graphics/scenes/SceneManager.hpp"
#include "graphics/scenes/SceneKeys.hpp"
#include "graphics/TextureManager.hpp"

void GameApp::run()
{
    InitWindow(1600, 900, "HORRIFIED");
    SetTargetFPS(60);
    AudioManager::getInstance().init();

    SceneManager &sm = SceneManager::getInstance();
    sm.registerAllScenes();
    sm.goTo(SceneKeys::INTRO_SCENE);

    while (!WindowShouldClose() && !sm.isExitRequested())
    {
        float dt = GetFrameTime();
        sm.update(dt);
        BeginDrawing();
        sm.render();
        EndDrawing();
    }

    TextureManager::getInstance().unloadAll();
    AudioManager::getInstance().dispose();
    CloseWindow();
}