#include "Hero.hpp"
#include <algorithm>
#include <iostream>

using namespace std;

Hero::Hero(const string &name, Location *startingLocation, int maxActions)
    : name(name),
      location(startingLocation),
      maxActions(maxActions),
      actionsLeft(maxActions) {}

void Hero::move(Location *destination, const vector<Villager *> &villagers)
{
    if (!hasActionsLeft())
    { // بررسی وجود اکشن
        throw GameException("No actions left!\n");
    }
    if (!location->isNeighbor(destination))
    { // بررسی مجاور بودن خونه انتخاب شده
        throw GameException("Destination is not adjacent to current location!\n");
    }

    Location *previousLocation = location; // کپی کردن لوکیشن اولیه برای بررسی محلی ها
    location = destination;                // انتقال قهرمان

    for (Villager *v : villagers) // انتقال محلی ها
    {
        if (v != nullptr && v->getCurrentLocation() == previousLocation)
        { // محلی معتبر بوده و با قهرمان در یک خانه بوده اند

            v->setLocation(destination);

            if (find(guideVillagers.begin(), guideVillagers.end(), v) == guideVillagers.end())
            {
                // محلی های تکراری به لیست اضافه نمی شوند
                guideVillagers.push_back(v);
            }
        }
    }

    useAction();
    updateGuideVillagers();
}

void Hero::guide(Villager *villager, Location *destination)
{
    if (!hasActionsLeft())
    { // بررسی وجود اکشن
        throw GameException("No actions left!\n");
    }
    if (!villager || !destination)
    { // بررسی معتبر بودن ورودی ها
        throw GameException("Invalid villager or destination!\n");
    }

    Location *villagerLocation = villager->getCurrentLocation();

    if (villagerLocation->isNeighbor(location) && destination == location)
    { // بررسی مجاور بودن محلی نسبت به قهرمان
        villager->setLocation(location);

        if (find(guideVillagers.begin(), guideVillagers.end(), villager) == guideVillagers.end())
        {
            guideVillagers.push_back(villager);
        }

        useAction();
        updateGuideVillagers();
    }

    if (villagerLocation == location && location->isNeighbor(destination))
    { // بررسی هم مکان بودن محلی و قهرمان
        villager->setLocation(destination);
        useAction();
        updateGuideVillagers();
    }
    else
    {
        throw GameException("Invalid guide action : check adjacency or location!\n");
    }
}

void Hero::pickUp(const vector<Item *> &pickedItems)
{
    if (!hasActionsLeft())
    { // بررسی وجود اکشن
        throw GameException("No actions left!\n");
    }
    if (!location)
    {
        throw GameException("Invalid current location!\n");
    }

    const vector<Item *> availableItems = location->get_items(); // گرفتن لیست آیتم های مکان فعلی

    int pickedCount = 0;

    for (Item *item : pickedItems)
    {
        if (item && find(availableItems.begin(), availableItems.end(), item) != availableItems.end())
        {
            items.push_back(item);
            location->removeItem(item);
            pickedCount++;
        }
        else
        {
            cout << "Item " << item << "is not available at current location: skipped.\n";
        }
    }
    if (pickedCount > 0)
    {
        useAction();
        cout << "Picked up " << pickedCount << " items.\n";
    }
    else
    {
        throw GameException("No valid items were picked up!\n");
    }
}

void Hero::advanced(Monster *monster)
{
    if (!hasActionsLeft())
    { // بررسی وجود اکشن
        throw GameException("No actions left!\n");
    }
    if (!monster)
    {
        throw GameException("Invalid monster pointer!\n");
    }
    if (monster->canbedefeated())
    { // بررسی اینکه آیا همه Advance ها انجام شده اند یا نه
        throw GameException("This monster's task is already complete!\n");
    }

    const vector<Location *> &validLocation = monster->getAdvanceLocation(); // در چه مکان هایی میتوان Advance انجام داد

    if (find(validLocation.begin(), validLocation.end(), location) == validLocation.end())
    {
        // بررسی اینکه قهرمان در مکان مجاز به advance هست یا نه
        throw GameException("You are not in a valid locayion to advance this monster's task!\n");
    }

    const vector<Item> &requiredItems = monster->getAdvanceRequirement(); // آیتم های مورد نیاز برای Advance

    if (!hasRequiredItems(requiredItems))
    { // بررسی اینکه قهرمان آیتم های مورد نیاز رو داره یا نه
        throw GameException("You don't have the required items to advance this monster's task!\n");
    }

    consumeItems(requiredItems);
    monster->advanceTask();
    useAction();
    cout << "Advanced monster's task successfully!\n";
}

void Hero::defeat(Monster *monster)
{
    if (!hasActionsLeft())
    { // بررسی وجود اکشن
        throw GameException("No actions left!\n");
    }
    if (!monster)
    {
        throw GameException("Invalid monster pointer!\n");
    }

    if (!monster->canbedefeated())
    { // بررسی کامل شدن Advance هیولا
        throw GameException("cannot defeat this monster : Advance task not complete!\n");
    }

    if (location != monster->get_location())
    { // آیا هیولا و قهرمان برای Defeat کردن  در یک مکان هستند
        throw GameException("Monster is not at your location!\n");
    }

    const vector<Item> &defeatItems = monster->getDefeatRequirement();

    if (!hasRequiredItems(defeatItems))
    { // بررسی وجود آیتم های لازم برای Defeat
        throw GameException("You don't have the required items to defeat this monster!\n");
    }

    useAction();
    consumeItems(monster->getDefeatRequirement());
    monster->set_defeated(true);
    cout << "Monster defeated successfully!\n";
}

void Hero::resetActions()
{
    actionsLeft = maxActions;
}

void Hero::useAction()
{
    if (actionsLeft > 0)
    {
        actionsLeft--;
    }
}

bool Hero::hasActionsLeft() const
{
    return actionsLeft > 0;
}

string Hero::getName() const
{
    return name;
}

Location *Hero::getLocation() const
{
    return location;
}

int Hero::getActionsLeft() const
{
    return actionsLeft;
}

vector<Item *> Hero::getItems() const
{
    return items;
}

vector<Villager *> Hero::getGuideVillagers() const
{
    return guideVillagers;
}

bool Hero::hasRequiredItems(const vector<Item> &required) const
{ // بررسی اینکه قهرمان آیتم های مورد نیاز برای Advance یا DEfeat رو داره یا نه
    {
        if (required.size() == 1 && required[0].get_name() == "AnyRedItemSum6")
        { // بررسی اینکه برای دراکورلاست یا نه
            int total = 0;

            for (Item *item : items)
            {
                if (item && item->get_color() == Color::red)
                {
                    total += item->get_strength();
                }
            }
            return total >= 6;
        }

        if (required.size() == 1 && required[0].get_name() == "AdvanceToken" && required[0].get_location())
        { // بررسی اینکه برای مرد نامریی است یا نه

            string requiredOrigin = required[0].get_location()->get_name();

            for (Item *item : items)
            {
                if (item && item->get_location() && item->get_location()->get_name() == requiredOrigin)
                {
                    return true;
                }
            }
            return false;
        }

        for (const Item &req : required)
        { // برای Defeat

            Color requiredType = req.get_color();
            int requiredPower = req.get_strength();
            int collectedPower = 0;

            for (Item *item : items)
            {
                if (item && item->get_color() == requiredType)
                {
                    collectedPower += item->get_strength();
                }
            }
            if (collectedPower < requiredPower)
            {
                return false;
            }
        }
        return true;
    }
}

void Hero::consumeItems(const vector<Item> &requiredItems)
{ // حذف کردن آیتم های مصرف شده

    ItemManager &itemManager = ItemManager::getInstance();

    if (requiredItems.size() == 1 && requiredItems[0].get_name() == "AnyRedItemSum6")
    {
        int needed = 6;
        for (auto it = items.begin(); it != items.end() && needed > 0;)
        {
            if ((*it)->get_color() == Color::red)
            {
                needed -= (*it)->get_strength();
                itemManager.recycleItem(*it);
                it = items.erase(it);
            }
            else
            {
                ++it;
            }
        }
        if (needed > 0)
        {
            throw GameException("Red item power not enough to consume for Dracula!\n");
        }
        return;
    }

    if (requiredItems.size() == 1 && requiredItems[0].get_name() == "AdvanceToken" && requiredItems[0].get_location())
    {
        string requiredOrigin = requiredItems[0].get_location()->get_name();

        for (auto it = items.begin(); it != items.end(); ++it)
        {
            if ((*it)->get_location() && (*it)->get_location()->get_name() == requiredOrigin)
            {
                itemManager.recycleItem(*it);
                items.erase(it);
                return;
            }
        }
        throw GameException("No matching item found to consume for Invisible Man!\n");
    }

    for (const Item &required : requiredItems)
    {
        Color targetColor = required.get_color();
        int targetPower = required.get_strength();
        int currentPower = 0;
        
        for (auto it = items.begin(); it != items.end() && currentPower < targetPower;)
        {
            if ((*it)->get_color() == targetColor)
            {
                currentPower += (*it)->get_strength();
                itemManager.recycleItem(*it);
                it = items.erase(it);
            }
            else
            {
                ++it;
            }
        }
        if (currentPower < targetPower)
        {
            throw GameException("Not enough item power to meet the requirement!\n");
        }
    }
}

void Hero::updateGuideVillagers()
{
    vector<Villager *> stillGuided;
    for (Villager *v : guideVillagers)
    {
        if (v && v->getCurrentLocation() == location && !v->isSafeLocation())
        {
            stillGuided.push_back(v);
        }
    }
    guideVillagers = stillGuided;
}

void Hero::setLocation(Location *newLocation)
{
    location = newLocation;
}