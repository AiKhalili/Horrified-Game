#ifndef SCENE_HPP
#define SCENE_HPP

class Scene{
public:
    virtual void update(float deleteTime) = 0;
    virtual void render() = 0;
    virtual ~Scene() = default;
};

#endif