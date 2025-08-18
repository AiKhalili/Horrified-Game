#ifndef PERKCARD_HPP
#define PERKCARD_HPP

#include <string>
#include <iostream>

enum class PerkType
{
    VISIT_FROM_THE_DETECTIVE,
    BREAK_OF_DAWN,
    OVERSTOCK,
    LATE_INTO_THE_NIGHT,
    REPEL,
    HURRY
};

class PerkCard
{
private:
    PerkType type;

public:
    PerkCard();
    PerkCard(PerkType);
    PerkType getType() const;
    std::string getName() const;
    std::string getDescription() const;

    std::string serialize() const;
    static PerkCard deserialize(const std::string &line);

};

#endif