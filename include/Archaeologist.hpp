#ifndef ARCHAEOLOGIST_HPP
#define ARCHAEOLOGIST_HPP

#include "Hero.hpp"
#include "Map.hpp"

class Archaeologist : public Hero{
public:
    Archaeologist(const std::string&, Map*);
    void specialAction(const std::vector<Item*> &)override;
    std::string getClassName() const override;
};

#endif