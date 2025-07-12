#ifndef COURIER_HPP
#define COURIER_HPP

#include "core/Hero.hpp"
#include "core/Map.hpp"

class Courier : public Hero
{
public:
    Courier(const std::string &, Map *);
    void specialAction(const std::vector<Item *> &) override;
    std::string getClassName() const override;
};

#endif