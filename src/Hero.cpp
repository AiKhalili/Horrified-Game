#include "Hero.hpp"
#include "PerkCard.hpp"
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
        }
    }
    useAction();
}

void Hero::guide(vector<Villager *> villager, Location *destination)
{
    if (!hasActionsLeft())
    { // بررسی وجود اکشن
        throw GameException("No actions left!\n");
    }
    if (!villager[0] || !destination)
    { // بررسی معتبر بودن ورودی ها
        throw GameException("Invalid villager or destination!\n");
    }
    if (villager.size() > 1)
    {
        throw GameException("You should choose one villager to guide!\n");
    }

    Location *villagerLocation = villager[0]->getCurrentLocation();

    if (villagerLocation->isNeighbor(location) && destination == location)
    { // بررسی مجاور بودن محلی نسبت به قهرمان
        villager[0]->setLocation(location);

        useAction();
    }

    else if (villagerLocation == location && location->isNeighbor(destination))
    { // بررسی هم مکان بودن محلی و قهرمان
        villager[0]->setLocation(destination);
        useAction();
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
        if (!item)
        {
            continue;
        }
        if (item && find(availableItems.begin(), availableItems.end(), item) != availableItems.end())
        {
            items.push_back(item);
            location->removeItem(item);
            pickedCount++;
        }
        else
        {
            tui.showMessageScreen("Item " + item->get_name() + "is not available at current location: skipped.");
        }
    }
    if (pickedCount > 0)
    {
        useAction();
        tui.showMessageScreen("Picked up " + to_string(pickedCount) + " items.");
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
    if (!requiredItems[0].get_location())
    {
        throw GameException("AdavnceToken has no valid location!");
    }

    if (monster->get_name() == "InvisibleMan" && requiredItems.size() == 1 &&
        requiredItems[0].get_name() == "AdvanceToken" && requiredItems[0].get_location())
    { // تیک مربوطه اگه هیولا مرد نامرییه

        Location *origin = requiredItems[0].get_location();
        monster->advanceMission(origin);
    }

    consumeItems(requiredItems);
    monster->advanceMission(location);
    useAction();
    tui.showMessageScreen("Advanced monster's task successfully!");
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
    consumeItems(defeatItems);
    monster->set_defeated(true);
    tui.showMessageScreen("Monster defeated successfully!");
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

bool Hero::hasRequiredItems(const std::vector<Item> &required) const
{
    if (required.size() == 1)
    {
        std::string tag = required[0].get_name();
        if (tag == "AnyRedItemSum6") // Advance دراکولا
        {
            int total = 0;
            for (Item *item : items)
                if (item && item->get_color() == COlOR::red)
                    total += item->get_strength();
            return total >= 6;
        }
        if (tag == "AdvanceToken" && required[0].get_location()) // Advance مرد نامرئی
        {
            std::string origin = required[0].get_location()->get_name();
            for (Item *item : items)
                if (item && item->get_location() && item->get_location()->get_name() == origin)
                    return true;
            return false;
        }
        if (tag == "DefeatDracula") // Defeat دراکولا
        {
            int total = 0;
            for (Item *item : items)
                if (item && item->get_color() == COlOR::yellow)
                    total += item->get_strength();
            return total >= 6;
        }
        if (tag == "DefeatInvisible") // Defeat مرد نامرئی
        {
            int total = 0;
            for (Item *item : items)
                if (item && item->get_color() == COlOR::red)
                    total += item->get_strength();
            return total >= 9;
        }
    }
    return false;
}

void Hero::consumeItems(const std::vector<Item> &requiredItems)
{
    ItemManager &itemManager = ItemManager::getInstance();

    if (requiredItems.size() == 1 && requiredItems[0].get_name() == "AnyRedItemSum6")
    {
        // مصرف آیتم‌های قرمز مجموع ≥ 6 برای Advance دراکولا
        std::vector<Item *> redItems;
        for (Item *item : items)
        {
            if (item && item->get_color() == COlOR::red)
                redItems.push_back(item);
        }

        // مرتب‌سازی بر اساس قدرت از کم به زیاد
        std::sort(redItems.begin(), redItems.end(), [](Item *a, Item *b)
                  { return a->get_strength() < b->get_strength(); });

        int total = 0;
        std::vector<Item *> toConsume;
        for (Item *item : redItems)
        {
            if (total >= 6)
                break;
            total += item->get_strength();
            toConsume.push_back(item);
        }

        if (total < 6)
            throw GameException("Red item power not enough to consume for Dracula!\n");

        // حذف آیتم‌ها
        for (Item *item : toConsume)
        {
            itemManager.recycleItemToUsedItems(item);
            items.erase(std::find(items.begin(), items.end(), item));
        }
        return;
    }

    if (requiredItems.size() == 1 &&
        requiredItems[0].get_name() == "AdvanceToken" &&
        requiredItems[0].get_location())
    {
        // برای Advance مرد نامریی (بر اساس مکان)
        std::string requiredOrigin = requiredItems[0].get_location()->get_name();

        for (auto it = items.begin(); it != items.end(); ++it)
        {
            if ((*it)->get_location() && (*it)->get_location()->get_name() == requiredOrigin)
            {
                itemManager.recycleItemToUsedItems(*it);
                items.erase(it);
                return;
            }
        }
        throw GameException("No matching item found to consume for Invisible Man!\n");
    }

    // مصرف آیتم‌ها در defeatها یا سایر موارد معمول
    for (const Item &required : requiredItems)
    {
        COlOR targetColor = required.get_color();
        int targetPower = required.get_strength();

        std::vector<Item *> matchedItems;
        int totalPower = 0;

        // جمع‌آوری آیتم‌های مناسب
        for (Item *item : items)
        {
            if (item && item->get_color() == targetColor)
            {
                matchedItems.push_back(item);
                totalPower += item->get_strength();
                if (totalPower >= targetPower)
                    break;
            }
        }

        if (totalPower < targetPower)
            throw GameException("Not enough item power to meet the requirement!\n");

        // حذف آیتم‌ها (دقیقاً به اندازه نیاز)
        int remainingPower = targetPower;
        for (Item *item : matchedItems)
        {
            itemManager.recycleItemToUsedItems(item);
            items.erase(std::find(items.begin(), items.end(), item));
            remainingPower -= item->get_strength();
            if (remainingPower <= 0)
                break;
        }
    }
}

void Hero::setLocation(Location *newLocation)
{
    location = newLocation;
}

void Hero::addPerkCard(const PerkCard &card)
{
    perkcards.push_back(card);
}

vector<PerkCard> Hero::getPerkCard() const
{
    return perkcards;
}

string Hero::getItemSummary() const
{
    if (perkcards.empty())
    {
        return "None";
    }

    string result = "";

    for (const auto &item : items)
    {
        result += item->get_name();
        result += "(";
        result += to_string(item->get_strength());
        result += ")\n";
    }
    return result;
}

int Hero::getMaxActions() const
{
    return maxActions;
}

string Hero::getPerkSummary() const
{
    if (items.empty())
    {
        return "None";
    }

    string result = "";

    for (const auto &perk : perkcards)
    {
        result += perk.getName();
        result += "\n";
    }
    return result;
}

PerkCard Hero::usePerkCard(int index)
{
    if (index < 0 || index >= static_cast<int>(perkcards.size()))
    {
        throw GameException("Invalid perk card index!");
    }

    PerkCard card = perkcards[index];
    perkcards.erase(perkcards.begin() + index);
    return card;
}

void Hero::addAction(int count)
{
    actionsLeft += count;
}

void Hero::removeItem(Item *item)
{
    auto it = find(items.begin(), items.end(), item);
    if (it != items.end())
    {
        items.erase(it);
    }
    else
    {
        throw GameException("Item not found in hero's inventory!");
    }
}

vector<string> Hero::getAction() const
{
    return {"Move", "Guide", "Pick Up", "Advance", "Defeat"};
}