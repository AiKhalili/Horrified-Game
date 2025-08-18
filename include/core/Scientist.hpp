#ifndef SCIENTIST_HPP
#define SCIENTIST_HPP

#include "core/Hero.hpp"
#include "core/Map.hpp"

class Scientist : public Hero
{
private:
    bool isAbilityActive = false;

public:
    Scientist(const std::string &, Map *);
    void specialAction(const std::vector<Item *> &) override;
    std::string getClassName() const override;
    void advanced(Monster *, const std::vector<Item *> &) override;
    void defeat(Monster *, const std::vector<Item *> &) override;
    void activateAbility(); // فعالسازی ابیلیتی
};

#endif