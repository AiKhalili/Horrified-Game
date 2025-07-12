#include "core/Scientist.hpp"
using namespace std;

Scientist::Scientist(const string &name, Map *map)
    : Hero(name, map ? map->getLocation("Institute") : throw GameException("Map is null!"), 4) {}

string Scientist::getClassName() const
{
    return "Scientist";
}

void Scientist::specialAction(const vector<Item *> &)
{
    throw GameException("Scientist has no special action!\n");
}

void Scientist::activateAbility()
{
    isAbilityActive = true;
    cout << "[Scientist] Ability activated: all selected Items will be +1.\n";
}

void Scientist::advanced(Monster *monster, const vector<Item *> &selectedItems)
{
    if (!isAbilityActive)
    { // بررسی اینکه ابیلیتی فعال شده یا نه ; اگه فعال نشده نسخه معمولی تابع کال میشه
        Hero::advanced(monster, selectedItems);
        return;
    }

    vector<Item *> boostedItems;
    for (Item *item : selectedItems)
    {
        if (!item)
        {
            continue;
        }
        Item *copy = new Item(*item);
        copy->set_strength(item->get_strength() + 1);
        boostedItems.push_back(copy);
    }

    // اجرای advance با ایتم های تقویت شده
    Hero::advanced(monster, boostedItems);

    for (Item *item : boostedItems)
    {
        delete item;
    }
    isAbilityActive = false;
}

void Scientist::defeat(Monster *monster, const vector<Item *> &selectedItems)
{
    if (!isAbilityActive)
    { // بررسی اینکه ابیلیتی فعال شده یا نه ; اگه فعال نشده نسخه معمولی تابع کال میشه
        Hero::defeat(monster, selectedItems);
        return;
    }

    vector<Item *> boostedItems;
    for (Item *item : selectedItems)
    {
        if (!item)
        {
            continue;
        }
        Item *copy = new Item(*item);
        copy->set_strength(item->get_strength() + 1);
        boostedItems.push_back(copy);
    }

    // اجرای advance با ایتم های تقویت شده
    Hero::defeat(monster, boostedItems);

    for (Item *item : boostedItems)
    {
        delete item;
    }
    isAbilityActive = false;
}