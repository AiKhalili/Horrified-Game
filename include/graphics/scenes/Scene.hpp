#ifndef SCENE_HPP
#define SCENE_HPP

class Scene
{
private:
    bool skipExitEnter = false;

public:
    virtual void update(float deleteTime) = 0;
    virtual void render() = 0;
    virtual void onEnter() {} // when you enter the scene
    virtual void onExit() {}  // when you exit the scene
    virtual ~Scene() = default;

    void setSkip(bool skip) { skipExitEnter = skip; }
    bool getSkip() const { return skipExitEnter; }
};

#endif