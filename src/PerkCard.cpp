#include "PerkCard.hpp"

using namespace std;

PerkCard::PerkCard(PerkType type) : type(type) {}

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
