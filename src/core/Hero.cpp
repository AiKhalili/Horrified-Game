#include "core/Hero.hpp"
#include "core/Monster.hpp"
#include "core/PerkCard.hpp"
#include "core/Map.hpp"
#include "core/Game.hpp"
#include <algorithm>
#include <iostream>
#include <sstream>
#include <fstream>

using namespace std;

Hero::Hero(const string &name, Location *startingLocation, int maxActions)
    : name(name),
      location(startingLocation),
      maxActions(maxActions),
      actionsLeft(maxActions)
{
    if (!startingLocation)
    {
        throw GameException("Starting location is null!");
    }

    // اضافه کردن قهرمان به مکان اولیه
    startingLocation->addHero(this);
}

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

    previousLocation->removeHero(this);
    destination->addHero(this);

    for (Villager *v : villagers) // انتقال محلی ها
    {
        if (v && v->getCurrentLocation() == previousLocation)
        { // محلی معتبر بوده و با قهرمان در یک خانه بوده اند
            previousLocation->removeVillager(v);
            destination->addVillager(v);
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
        villager[0]->getCurrentLocation()->removeVillager(villager[0]);
        location->addVillager(villager[0]);
        useAction();
        cout << "The select villager move to the hero's place!\n";
    }

    else if (villagerLocation == location && location->isNeighbor(destination))
    { // بررسی هم مکان بودن محلی و قهرمان
        villager[0]->getCurrentLocation()->removeVillager(villager[0]);
        destination->addVillager(villager[0]);
        useAction();
        cout << "The select villager move to the select place!\n";
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
            item->set_pickedFrom(location->get_name());
            location->removeItem(item);
            pickedCount++;
        }
        else
        {
            cout << "Item " << item->get_name() << "is not available at current location: skipped.\n";
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

void Hero::advanced(Monster *monster, const vector<Item *> &selectedItems)
{
    if (!hasActionsLeft())
    {
        throw GameException("No actions left!\n");
    }

    if (!monster)
    {
        throw GameException("Invalid monster pointer!\n");
    }

    if (selectedItems.empty())
    {
        throw GameException("Selected items are empty!\n");
    }

    if (monster->canbedefeated())
    {
        throw GameException("This monster's task is already complete!\n");
    }

    const vector<Location *> &validLocation = monster->getAdvanceLocation();

    if (find(validLocation.begin(), validLocation.end(), location) == validLocation.end())
    {
        throw GameException("You are not in a valid location to advance this monster's task!\n");
    }

    if (monster->get_name() == "InvisibleMan")
    {
        const vector<Item> &reqs = monster->getAdvanceRequirement(); // شامل لوکیشن‌هایی که هنوز advance نشدن
        for (const Item &required : reqs)
        {
            string targetLoc = required.get_location()->get_name();

            for (Item *it : selectedItems)
            {
                if (it && it->get_pickedFrom() == targetLoc)
                {
                    Location *advanceLoc = Map::get_instanse()->getLocation(targetLoc);
                    if (!advanceLoc)
                    {
                        throw GameException("Advance location not found: " + targetLoc + "\n");
                    }

                    // فقط اگر اون لوکیشن هنوز Advance نشده
                    if (monster->isAdvanceLocation(targetLoc))
                    {
                        monster->advanceMission(advanceLoc);
                        consumeItems({Item(0, COlOR::red, "AdvanceToken", advanceLoc)}, selectedItems);
                        useAction();
                        cout << "Advanced Invisible Man for location: " << targetLoc << "\n";
                        return;
                    }
                }
            }
        }

        throw GameException("You don't have the required items to advance this monster's task!\n");
    }

    // حالت عادی برای هیولاهای دیگه
    const vector<Item> &requiredItems = monster->getAdvanceRequirement();
    if (!hasRequiredItems(requiredItems, selectedItems))
    {
        throw GameException("You don't have the required items to advance this monster's task!\n");
    }

    consumeItems(requiredItems, selectedItems);
    monster->advanceMission(location);
    useAction();
    cout << "Advanced monster's task successfully!\n";
}

void Hero::defeat(Monster *monster, const vector<Item *> &selectedItems)
{
    if (!hasActionsLeft())
    { // بررسی وجود اکشن
        throw GameException("No actions left!\n");
    }
    if (!monster)
    {
        throw GameException("Invalid monster pointer!\n");
    }
    if (selectedItems.empty())
    {
        throw GameException("Selected items are empty!\n");
    }

    if (!monster->canbedefeated())
    { // بررسی کامل شدن Advance هیولا
        throw GameException("cannot defeat this monster : Advance task not complete!\n");
    }

    if (monster->is_defeated())
    {
        throw GameException("This monster is already defeated!\n");
    }

    if (location != monster->get_location())
    { // آیا هیولا و قهرمان برای Defeat کردن  در یک مکان هستند
        throw GameException("Monster is not at your location!\n");
    }

    const vector<Item> &defeatItems = monster->getDefeatRequirement();

    if (!hasRequiredItems(defeatItems, selectedItems))
    { // بررسی وجود آیتم های لازم برای Defeat
        throw GameException("You don't have the required items to defeat this monster!\n");
    }

    useAction();
    consumeItems(defeatItems, selectedItems);
    monster->set_defeated(true);

    if (monster == Game::getInstance().getFrenzy())
    {
        Game::getInstance().updateFrenzy();
    }
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

vector<Item *> & Hero::getItems()
{
    return items;
}

bool Hero::hasRequiredItems(const vector<Item> &required, const vector<Item *> &selectedItems) const
{
    if (required.empty() || selectedItems.empty())
    {
        return false;
    }

    string tag = required[0].get_name();

    if (required.size() == 1)
    {
        // Advance دراکولا: آیتم‌های قرمز مجموع ≥ 6
        if (tag == "AnyRedItemSum6")
        {
            int total = 0;
            for (Item *item : selectedItems)
                if (item && item->get_color() == COlOR::red)
                    total += item->get_strength();
            return total >= 6;
        }

        // Defeat دراکولا: آیتم زرد مجموع ≥ 6
        if (tag == "DefeatDracula")
        {
            int total = 0;
            for (Item *item : selectedItems)
                if (item && item->get_color() == COlOR::yellow)
                    total += item->get_strength();
            return total >= 6;
        }

        // Defeat مرد نامرئی: آیتم قرمز مجموع ≥ 9
        if (tag == "DefeatInvisible")
        {
            int total = 0;
            for (Item *item : selectedItems)
                if (item && item->get_color() == COlOR::red)
                    total += item->get_strength();
            return total >= 9;
        }
    }

    // Advance مرد نامرئی: آیتمی که از یکی از مکان‌های خاص برداشته شده باشد
    if (tag == "AdvanceToken" && required[0].get_location())
    {
        string requiredOrigin = required[0].get_location()->get_name();

        for (Item *item : selectedItems)
        {
            if (item && item->get_pickedFrom() == requiredOrigin)
            {
                return true;
            }
        }
        return false;
    }
    return false;
}

void Hero::consumeItems(const vector<Item> &requiredItems, const vector<Item *> &selectedItems)
{
    ItemManager &itemManager = ItemManager::getInstance();

    //  حالت خاص: Dracula - AnyRedItemSum6
    if (requiredItems.size() == 1 && requiredItems[0].get_name() == "AnyRedItemSum6")
    {
        vector<Item *> redItems;
        for (Item *item : selectedItems)
        {
            if (item && item->get_color() == COlOR::red)
                redItems.push_back(item);
        }

        int total = 0;
        for (Item *item : redItems)
        {
            total += item->get_strength();
        }

        if (total < 6)
            throw GameException("Red item power not enough to consume for Dracula!\n");

        for (Item *item : redItems)
        {
            itemManager.recycleItemToUsedItems(item);
            items.erase(find(items.begin(), items.end(), item));
        }
        return;
    }

    //  حالت خاص: Invisible Man - AdvanceToken + pickedFrom
    if (requiredItems.size() == 1 &&
        requiredItems[0].get_name() == "AdvanceToken" &&
        requiredItems[0].get_location())
    {
        string requiredOrigin = requiredItems[0].get_location()->get_name();

        // پیدا کردن اولین آیتم با pickedFrom همون location
        for (Item *item : selectedItems)
        {
            if (item && item->get_pickedFrom() == requiredOrigin)
            {
                itemManager.recycleItemToUsedItems(item);
                items.erase(find(items.begin(), items.end(), item));
                return;
            }
        }
        throw GameException("No matching item found to consume for Invisible Man!\n");
    }

    // حالت کلی defeat یا سایر هیولاها
    for (const Item &required : requiredItems)
    {
        COlOR targetColor = required.get_color();
        int targetPower = required.get_strength();

        vector<Item *> matchedItems;
        int totalPower = 0;

        for (Item *item : selectedItems)
        {
            if (item && item->get_color() == targetColor)
            {
                matchedItems.push_back(item);
                totalPower += item->get_strength();
            }
        }

        if (totalPower < targetPower)
        {
            throw GameException("Not enough item power to meet the requirement!\n");
        }

        for (Item *item : matchedItems)
        {
            itemManager.recycleItemToUsedItems(item);
            items.erase(find(items.begin(), items.end(), item));
        }
    }
}

void Hero::setLocation(Location *newLocation)
{
    location = newLocation;
}

void Hero::setPerkCard(const PerkCard &card)
{
    perkcards.push_back(card);
}

vector<PerkCard> Hero::getPerkCard() const
{
    return perkcards;
}

string Hero::getItemSummary() const
{
    if (items.empty())
    {
        return "None\n";
    }

    string result = "";

    for (size_t i = 0; i < items.size(); i++)
    {
        result += items[i]->get_name();
        result += "(";
        result += to_string(items[i]->get_strength());
        result += ")";
        result += "[";
        result += items[i]->get_color_to_string();
        result += "]";
        if (i == items.size() - 1)
        {
            result += "\n";
        }
        else
        {
            result += "\n\t\t";
        }
    }
    return result;
}

int Hero::getMaxActions() const
{
    return maxActions;
}

string Hero::getPerkSummary() const
{
    if (perkcards.empty())
    {
        return "None\n";
    }

    string result = "";

    for (size_t i = 0; i < perkcards.size(); i++)
    {
        result += to_string(i + 1);
        result += ".";
        result += perkcards[i].getName();
        if (i == perkcards.size() - 1)
        {
            result += "\n";
        }
        else
        {
            result += "\n\t\t";
        }
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

string Hero::serialize() const {
    string data = "Hero|";
    data += getClassName() + "|";                      // نوع قهرمان
    data += name + "|";                                // نام قهرمان
    data += location->get_name() + "|";                // مکان فعلی
    data += to_string(maxActions) + "|";
    data += to_string(actionsLeft) + "|";

    // آیتم‌ها
    for (size_t i = 0; i < items.size(); ++i) {
        data += items[i]->get_name() + "-" +
                to_string(items[i]->get_strength()) + "-" +
                items[i]->get_color_to_string() + "-" +
                items[i]->get_pickedFrom();
        if (i != items.size() - 1) data += ",";
    }
    data += "|";

    // کارت‌های پرک
    for (size_t i = 0; i < perkcards.size(); ++i) {
        data += perkcards[i].getName();
        if (i != perkcards.size() - 1) data += ",";
    }

    return data;
}



Hero* Hero::deserialize(const string& line) {
    stringstream ss(line);
    string part;

   if (line.substr(0, 5) != "Hero|") return nullptr;

    getline(ss, part, '|'); // "Hero"
    getline(ss, part, '|');
    string className = part;

    getline(ss, part, '|');
    string name = part;

    getline(ss, part, '|');
    Location* location = Map::get_instanse()->getLocation(part);
    if (!location) throw GameException("Invalid hero location: " + part);

    getline(ss, part, '|');
    int maxActions = part.empty() ? 0 : stoi(part);

    getline(ss, part, '|');
    int actionsLeft = part.empty() ? 0 : stoi(part);
    if (actionsLeft == 0) {
        actionsLeft = maxActions; // ← اگر صفر بود، از نو مقداردهی کن
    }

    // ساخت هیرو
    Hero* hero = nullptr;
    if (className == "Mayor") {
        hero = new Mayor(name, Map::get_instanse());
    } else if (className == "Archaeologist") {
        hero = new Archaeologist(name, Map::get_instanse());
    } 
    else if(className == "Courier"){
        hero = new Courier(name,Map::get_instanse());
    }
    else if(className == "Scientist"){
        hero = new Scientist(name, Map::get_instanse());
    }
    else {
        throw GameException("Unknown hero class: " + className);
    }

    // اطمینان از قرارگیری در مکان درست
    if (hero->getLocation() != location) {
        Map::get_instanse()->removeHeroFrom(hero->getLocation()->get_name(), hero);
        hero->setLocation(location);
        Map::get_instanse()->addHeroTo(location->get_name(), hero);
    }

    // تنظیم اکشن‌ها
    hero->setActionsLeft(actionsLeft);

    // آیتم‌ها
    getline(ss, part, '|');
    if (!part.empty()) {
        stringstream itemStream(part);
        string itemData;
        while (getline(itemStream, itemData, ',')) {
            stringstream itemParts(itemData);
            string itemName, strengthStr, colorStr, pickedFrom;

            getline(itemParts, itemName, '-');
            getline(itemParts, strengthStr, '-');
            getline(itemParts, colorStr, '-');
            getline(itemParts, pickedFrom);

            COlOR color;
            if (colorStr == "Red") color = COlOR::red;
            else if (colorStr == "Blue") color = COlOR::blue;
            else if (colorStr == "Yellow") color = COlOR::yellow;
            else throw GameException("Invalid color: " + colorStr);

            Location* from = Map::get_instanse()->getLocation(pickedFrom);
            if (!from) throw GameException("Invalid pickedFrom location: " + pickedFrom);

            Item* item = new Item(stoi(strengthStr), color, itemName, from);
            item->set_pickedFrom(pickedFrom);
            hero->getItems().push_back(item);
        }
    }

    // پرک‌کارت‌ها
    getline(ss, part, '|');
    if (!part.empty()) {
        stringstream perkStream(part);
        string perkName;
        while (getline(perkStream, perkName, ',')) {
            PerkType type;
            if (perkName == "VisitFromTheDetective") type = PerkType::VISIT_FROM_THE_DETECTIVE;
            else if (perkName == "BreakOfDawn")       type = PerkType::BREAK_OF_DAWN;
            else if (perkName == "Overstock")           type = PerkType::OVERSTOCK;
            else if (perkName == "LateIntoTheNight") type = PerkType::LATE_INTO_THE_NIGHT;
            else if (perkName == "Repel")               type = PerkType::REPEL;
            else if (perkName == "Hurry")               type = PerkType::HURRY;
            else throw GameException("Unknown perk: " + perkName);

            hero->addPerkCard(PerkCard(type));
        }
    }

    return hero;
}


void Hero::setActionsLeft(int actions) {
    this->actionsLeft = actions;
}

void Hero::addPerkCard(const PerkCard& card) {
    perkcards.push_back(card);
}