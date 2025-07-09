#ifndef MAYOR_HPP
#define MAYOR_HPP

#include "Hero.hpp"

class Map;

class Mayor : public Hero
{
public:
    Mayor(const std::string &, Map *);
    void specialAction(const std::vector<Item *> &) override; 
    std::string getClassName() const override;
};

#endif