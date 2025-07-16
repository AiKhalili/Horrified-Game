#ifndef SCENE_HPP
#define SCENE_HPP

class Scene
{
public:
    virtual void update(float deleteTime) = 0;
    virtual void render() = 0;
    virtual void onEnter() {} // when you enter the scene
    virtual void inExit() {}  // when you exit the scene
    virtual ~Scene() = default;
};

#endif