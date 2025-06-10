#ifndef MAYOR_HPP
#define MAYOR_HPP

#include "Hero.hpp"
#include "Map.hpp"

class Mayor : public Hero{
public:
    Mayor(const std::string&, Map*);
    void specialAction(const std::vector<Item*> &)override;
};

#endif