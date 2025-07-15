#include "core/PerkCard.hpp"

using namespace std;

PerkCard::PerkCard(PerkType type) : type(type) {}

PerkCard::PerkCard() {}

PerkType PerkCard::getType() const
{
    return type;
}

string PerkCard::getName() const
{
    switch (type)
    {
    case PerkType::VISIT_FROM_THE_DETECTIVE:
        return "Visit from the detective";
    case PerkType::BREAK_OF_DAWN:
        return "Break of dawn";
    case PerkType::OVERSTOCK:
        return "Overstock";
    case PerkType::LATE_INTO_THE_NIGHT:
        return "Late into the night";
    case PerkType::REPEL:
        return "Repel";
    case PerkType::HURRY:
        return "Hurry";
    default:
        return "Unkown";
    }
}

string PerkCard::getDescription() const
{
    switch (type)
    {
    case PerkType::VISIT_FROM_THE_DETECTIVE:
        return "Place the Invisible Man in any location of your choice on the map.";
    case PerkType::BREAK_OF_DAWN:
        return "Skip the next Monster Phase. Draw 2 items from the item bag and place them in their respective locations.";
    case PerkType::OVERSTOCK:
        return "Each player draws 1 item from the item bag and places it in their current location.";
    case PerkType::LATE_INTO_THE_NIGHT:
        return "The current player gains 2 additional actions this turn.";
    case PerkType::REPEL:
        return "Move any monster up to 2 spaces. You can choose any available paths; there are no restrictions.";
    case PerkType::HURRY:
        return "Move any hero up to 2 spaces. You can choose any available paths; there are no restrictions.";
    default:
        return "Unknown effect.";
    }
}

std::string PerkCard::serialize() const
{
    return "PerkCard|" + getName();
}
PerkCard PerkCard::deserialize(const std::string& line) {
    std::string cleanLine = line;

    while (!cleanLine.empty() && std::isspace(cleanLine.front()))
        cleanLine.erase(cleanLine.begin());
    while (!cleanLine.empty() && std::isspace(cleanLine.back()))
        cleanLine.pop_back();

    if (cleanLine.length() < 9) {
        throw std::runtime_error("Invalid perk card format: line too short");
    }

    if (cleanLine.substr(0, 9) != "PerkCard|") {
        throw std::runtime_error("Invalid perk card format: must start with 'PerkCard|'");
    }

    std::string name = cleanLine.substr(9);

    if (name == "Visit from the detective") return PerkCard(PerkType::VISIT_FROM_THE_DETECTIVE);
    if (name == "Break of dawn") return PerkCard(PerkType::BREAK_OF_DAWN);
    if (name == "Overstock") return PerkCard(PerkType::OVERSTOCK);
    if (name == "Late into the night") return PerkCard(PerkType::LATE_INTO_THE_NIGHT);
    if (name == "Repel") return PerkCard(PerkType::REPEL);
    if (name == "Hurry") return PerkCard(PerkType::HURRY);

    throw std::runtime_error("Unknown perk card name: '" + name + "'");
}
