#ifndef UIELEMENT_HPP
#define UIELEMENT_HPP

class UIElement
{
public:
    virtual void render() = 0;
    virtual void update() = 0;
    virtual ~UIElement() = default;
};

#endif