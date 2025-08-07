#include "saves/SaveManager.hpp"
#include "core/Map.hpp"
#include "core/Game.hpp"
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <iostream>
#include <core/Map.hpp>
#include "graphics/scenes/SceneManager.hpp"
#include "graphics/scenes/SceneKeys.hpp"
#include "graphics/scenes/ItemSelectionScene.hpp"
#include "graphics/scenes/VillagerSelectionScene.hpp"
#include "graphics/scenes/MonsterSelectionScene.hpp"
#include "graphics/scenes/PerkSelectionScene.hpp"
#include "graphics/scenes/PlayerSummaryScene.hpp"
#include "graphics/scenes/LocationSelectionScene.hpp"

SaveManager &SaveManager::getInstance()
{
    static SaveManager instance;
    return instance;
}

void SaveManager::saveGameToSlot(std::string newkey)
{
    int slotNumber;

    // اگر بازی از فایل لود شده روی همون فایل ذخیره کنیم
    if (Game::getInstance().getLoadedFromFile())
    {
        slotNumber = Game::getInstance().getCurrentSaveSlot();
    }
    else
    {
        slotNumber = Game::getInstance().findNextFreeSlot();
    }

    if (slotNumber < 1)
        throw GameException("slotNumber can not be less than 1!\n");

    std::string filename = "save" + std::to_string(slotNumber) + ".txt";

    saveHeroesToFile(filename, Game::getInstance().getHeroes());
    saveCurrentHeroStateToFile(filename, Game::getInstance().getCurrentHeroIndex(),
                               Game::getInstance().getHeroes()[Game::getInstance().getCurrentHeroIndex()]->getActionsLeft());
    saveMonstersToFile(filename, Game::getInstance().getMonsters());
    saveVillagersToFile(filename, Game::getInstance().getVillagers());
    auto items = Map::get_instanse()->getAllItems();
    saveItemsToFile(filename, items);
    saveMonsterCardsToFile(filename, Game::getInstance().getMonsterDeck());
    saveItemManagerStateToFile(filename);
    saveTerrorLevel(filename, Game::getInstance().getTerrorLevel());
    saveFrenzy(filename, Game::getInstance().getFrenzy()->get_name());
    savePerkDeckToFile(filename, Game::getInstance().getPerkDeck());
    writeSaveTimestamp(filename);

    Game::getInstance().setSlot(slotNumber);
    Game::getInstance().setCurrentSaveSlot(slotNumber);

    Game::getInstance().setLoadedFromFile(true);
    saveSceneToFile(filename, newkey);
}

void SaveManager::loadGameFromSlot(int slotNumber)
{
    if (slotNumber < 1)
        throw GameException("slotNumber can not be less than 1!\n");

    Game::getInstance().reset();

    std::string filename = "save" + std::to_string(slotNumber) + ".txt";

    std::ifstream inFile(filename);
    if (!inFile.is_open())
        throw std::runtime_error("No file has been saved for this slot!\n");

    std::vector<Hero *> heroes = loadHeroesFromFile(filename);
    Game::getInstance().setHeroes(heroes); // قهرمان رو در گیم ست می کنیم

    auto [currentIndex, actionsLeft] = loadCurrentHeroStateFromFile(filename);
    Game::getInstance().setCurrentHeroIndex(currentIndex);
    Game::getInstance().getHeroes()[Game::getInstance().getCurrentHeroIndex()]->setActionsLeft(actionsLeft);

    std::vector<Monster *> monsters = loadMonstersFromFile(filename);
    Game::getInstance().setMonsters(monsters);

    std::vector<Villager *> villagers = loadVillagersFromFile(filename);
    Game::getInstance().setVillagers(villagers);

    std::vector<Item *> items = loadItemsFromFile(filename);

    // اضافه کردن آیتم ها به مکان هاشون
    for (Item *item : items)
    {
        if (item->get_location())
            item->get_location()->addItem(item);
    }
    std::vector<MonsterCard> cards = loadMonsterCardsFromFile(filename);
    Game::getInstance().setMonsterDeck(cards);

    loadItemManagerStateFromFile(filename);

    int terror = loadTerrorLevel(filename);
    Game::getInstance().setTerrorLevel(terror);

    Monster *frenzy = loadFrenzy(filename, monsters);
    Game::getInstance().setFrenzyMonster(frenzy);

    std::vector<PerkCard> deck = loadPerkDeckFromFile(filename);
    Game::getInstance().setPerkDeck(deck);

    Game::getInstance().setLoadedFromFile(true);

    Game::getInstance().setSlot(slotNumber);
    Game::getInstance().setCurrentSaveSlot(slotNumber);

    SaveManager::getInstance().loadSceneFromFile(filename);

}

void SaveManager::saveHeroesToFile(const std::string &filename, const std::vector<Hero *> &heroes)
{
    std::ofstream outFile(filename);
    if (!outFile.is_open())
        throw std::runtime_error("Error opening file for saving!\n");

    for (const Hero *hero : heroes)
    {
        outFile << hero->serialize() << "\n";
    }

    outFile.close();
}

std::vector<Hero *> SaveManager::loadHeroesFromFile(const std::string &filename)
{
    std::ifstream inFile(filename);
    if (!inFile.is_open())
        throw std::runtime_error("The file cannot be opened for reading!\n");

    std::vector<Hero *> heroes;
    std::string line;
    while (std::getline(inFile, line))
    {
        Hero *hero = Hero::deserialize(line);
        if (hero != nullptr)
        {
            heroes.push_back(hero);
        }
    }
    return heroes;
}

void SaveManager::saveMonstersToFile(const std::string &filename, const std::vector<Monster *> &monsters)
{
    std::ofstream outFile(filename, std::ios::app); // حالت append برای ادامه دادن به فایل قهرمان‌ها
    if (!outFile.is_open())
        throw std::runtime_error("Error opening file for saving monsters!\n");

    for (const Monster *m : monsters)
    {
        outFile << m->serialize() << "\n";
    }

    outFile.close();
}

std::vector<Monster *> SaveManager::loadMonstersFromFile(const std::string &filename)
{
    std::ifstream inFile(filename);
    if (!inFile.is_open())
        throw std::runtime_error("Error opening file to load monsters!\n");

    std::vector<Monster *> monsters;
    std::string line;
    int heroLines = 2;
    while (heroLines-- && std::getline(inFile, line))
    {
    }

    while (std::getline(inFile, line))
    {
        if (line.empty())
            continue;

        if (line.rfind("Dracula|", 0) == 0 || line.rfind("InvisibleMan|", 0) == 0)
        {
            Monster *monster = Monster::deserialize(line);
            if (monster != nullptr) // فقط مانسترهایی که location داشتن
                monsters.push_back(monster);
        }
    }

    return monsters;
}

void SaveManager::saveVillagersToFile(const std::string &filename, const std::vector<Villager *> &villagers)
{
    std::ofstream outFile(filename, std::ios::app); // append
    if (!outFile.is_open())
        throw std::runtime_error("Error opening file for saving villagers!\n");

    for (const Villager *v : villagers)
    {
        outFile << v->serialize() << "\n";
    }

    outFile.close();
}

std::vector<Villager *> SaveManager::loadVillagersFromFile(const std::string &filename)
{
    std::ifstream inFile(filename);
    if (!inFile.is_open())
        throw std::runtime_error("Cannot open file to load villagers!\n");

    std::vector<Villager *> villagers;
    std::string line;

    int skipLines = 2 + 2;
    while (skipLines-- && std::getline(inFile, line))
    {
    }

    while (std::getline(inFile, line))
    {
        if (line.rfind("Villager|", 0) == 0)
        {
            Villager *v = Villager::deserialize(line);
            villagers.push_back(v);
        }
    }

    return villagers;
}

void SaveManager::saveItemsToFile(const std::string &filename, const std::vector<Item *> &items)
{
    std::ofstream outFile(filename, std::ios::app); // append
    if (!outFile.is_open())
        throw std::runtime_error("Error opening file for saving items!\n");

    for (const Item *item : items)
    {
        outFile << item->serialize() << "\n";
    }

    outFile.close();
}

std::vector<Item *> SaveManager::loadItemsFromFile(const std::string &filename)
{
    std::ifstream inFile(filename);
    if (!inFile.is_open())
        throw std::runtime_error("Cannot open file to load items!\n");

    std::vector<Item *> items;
    std::string line;

    while (std::getline(inFile, line))
    {
        if (line.rfind("Item|", 0) == 0)
        {
            Item *item = Item::deserialize(line);
            items.push_back(item);
        }
    }

    return items;
}

void SaveManager::saveMonsterCardsToFile(const std::string &filename, const std::vector<MonsterCard> &cards)
{
    std::ofstream outFile(filename, std::ios::app); // append
    if (!outFile.is_open())
        throw std::runtime_error("Error opening file for saving monster cards!\n");

    for (const MonsterCard &card : cards)
    {
        outFile << card.serialize() << "\n";
    }

    outFile.close();
}

std::vector<MonsterCard> SaveManager::loadMonsterCardsFromFile(const std::string &filename)
{
    std::ifstream inFile(filename);
    if (!inFile.is_open())
        throw std::runtime_error("Cannot open file to load monster cards!\n");

    std::vector<MonsterCard> cards;
    std::string line;

    while (std::getline(inFile, line))
    {
        if (line.rfind("MonsterCard|", 0) == 0)
        {
            cards.push_back(MonsterCard::deserialize(line));
        }
    }

    return cards;
}

void SaveManager::saveItemManagerStateToFile(const std::string &filename)
{
    std::ofstream outFile(filename, std::ios::app); // append
    if (!outFile.is_open())
        throw std::runtime_error("Cannot open file to save ItemManager state!\n");

    outFile << ItemManager::getInstance().serializeBag() << "\n";
    outFile << ItemManager::getInstance().serializeUsed() << "\n";

    outFile.close();
}

void SaveManager::loadItemManagerStateFromFile(const std::string &filename)
{
    std::ifstream inFile(filename);
    if (!inFile.is_open())
        throw std::runtime_error("Cannot open file to load ItemManager state!\n");

    std::string line;
    while (std::getline(inFile, line))
    {
        if (line.rfind("ItemBag|", 0) == 0)
            ItemManager::getInstance().deserializeBag(line);
        else if (line.rfind("ItemUsed|", 0) == 0)
            ItemManager::getInstance().deserializeUsed(line);
    }

    inFile.close();
}

void SaveManager::saveTerrorLevel(const std::string &filename, int level)
{
    std::ofstream outFile(filename, std::ios::app); // append
    if (!outFile.is_open())
        throw std::runtime_error("Cannot open file to save Terror Level!\n");

    outFile << "Terror|" << level << "\n";
    outFile.close();
}

int SaveManager::loadTerrorLevel(const std::string &filename)
{
    std::ifstream inFile(filename);
    if (!inFile.is_open())
        throw std::runtime_error("Cannot open file to load Terror Level!\n");

    std::string line;
    while (std::getline(inFile, line))
    {
        if (line.rfind("Terror|", 0) == 0)
        {
            std::string levelStr = line.substr(7);
            return std::stoi(levelStr);
        }
    }

    return 0;
}

void SaveManager::saveCurrentTurn(const std::string &filename, int index)
{
    std::ofstream outFile(filename, std::ios::app);
    outFile << "CurrentTurn|" << index << "\n";
}

int SaveManager::loadCurrentTurn(const std::string &filename)
{
    std::ifstream inFile(filename);
    std::string line;
    while (std::getline(inFile, line))
    {
        if (line.rfind("CurrentTurn|", 0) == 0)
            return std::stoi(line.substr(12));
    }
    return 0;
}

void SaveManager::saveTimes(const std::string &filename, int t1, int t2)
{
    std::ofstream outFile(filename, std::ios::app);
    if (!outFile.is_open())
        throw std::runtime_error("Cannot open file to save times!\n");

    outFile << "Times|" << t1 << "," << t2 << "\n";
    outFile.close();
}

void SaveManager::loadTimes(const std::string &filename, int &t1, int &t2)
{
    std::ifstream inFile(filename);
    if (!inFile.is_open())
        throw std::runtime_error("Cannot open file to load times!\n");

    std::string line;
    while (std::getline(inFile, line))
    {
        if (line.rfind("Times|", 0) == 0)
        {
            std::string rest = line.substr(6);
            size_t commaPos = rest.find(',');
            t1 = std::stoi(rest.substr(0, commaPos));
            t2 = std::stoi(rest.substr(commaPos + 1));
            return;
        }
    }

    t1 = t2 = 0;
}

void SaveManager::saveFrenzy(const std::string &filename, const std::string &monsterName)
{
    std::ofstream outFile(filename, std::ios::app); // append
    if (!outFile.is_open())
        throw std::runtime_error("Error opening file for saving Frenzy!\n");

    outFile << "Frenzy|" << monsterName << "\n";
    outFile.close();
}

Monster *SaveManager::loadFrenzy(const std::string &filename, const std::vector<Monster *> &monsters)
{
    std::ifstream inFile(filename);
    if (!inFile.is_open())
        throw std::runtime_error("Cannot open file to load Frenzy!\n");

    std::string line;
    while (std::getline(inFile, line))
    {
        if (line.rfind("Frenzy|", 0) == 0)
        {
            std::string name = line.substr(7); // بعد از "Frenzy|"
            for (Monster *m : monsters)
            {
                if (m && m->get_name() == name)
                    return m;
            }
            throw GameException("Monster with name '" + name + "' not found to set as Frenzy!");
        }
    }

    return nullptr; // اگر پیدا نشد
}

void SaveManager::savePerkDeckToFile(const std::string &filename, const std::vector<PerkCard> &deck)
{
    std::ofstream outFile(filename, std::ios::app); // append
    if (!outFile.is_open())
        throw std::runtime_error("Error opening file to save perk deck!");

    for (const PerkCard &card : deck)
    {
        outFile << "PerkCard|" << card.getName() << "\n";
    }
}

std::vector<PerkCard> SaveManager::loadPerkDeckFromFile(const std::string &filename)
{
    std::ifstream inFile(filename);
    if (!inFile.is_open())
        throw std::runtime_error("Cannot open file to load perk cards!\n");

    std::vector<PerkCard> cards;
    std::string line;

    while (std::getline(inFile, line))
    {
        // فقط اگر خط با PerkCard| شروع شد، دی‌سریالایز کن
        if (line.rfind("PerkCard|", 0) == 0)
        {
            cards.push_back(PerkCard::deserialize(line));
        }
    }

    return cards;
}

void SaveManager::saveCurrentHeroStateToFile(const std::string &filename, int currentIndex, int actionsLeft)
{
    std::ofstream outFile(filename, std::ios::app); // append
    if (!outFile.is_open())
        throw GameException("Cannot open file to save current hero state!");

    outFile << "CurrentHeroState|" << currentIndex << "|" << actionsLeft << '\n';
}

std::pair<int, int> SaveManager::loadCurrentHeroStateFromFile(const std::string &filename)
{
    std::ifstream inFile(filename);
    if (!inFile.is_open())
        throw GameException("Cannot open file to load current hero state!");

    std::string line;
    while (std::getline(inFile, line))
    {
        if (line.find("CurrentHeroState|") == 0)
        {
            std::stringstream ss(line);
            std::string token;
            getline(ss, token, '|'); // "CurrentHeroState"
            getline(ss, token, '|'); // index
            int index = stoi(token);
            getline(ss, token, '|'); // actionsLeft
            int actions = stoi(token);
            return {index, actions};
        }
    }

    throw GameException("CurrentHeroState not found in save file!");
}

void SaveManager::writeSaveTimestamp(const std::string &filename)
{
    std::ofstream outFile(filename, std::ios::app); // حالت append یعنی اضافه کردن به آخر فایل
    if (!outFile.is_open())
        throw std::runtime_error("Failed to open save file for writing timestamp");

    std::time_t now = std::time(nullptr);
    std::tm localTime = *std::localtime(&now);
    outFile << "SaveDateTime: " << std::put_time(&localTime, "%Y-%m-%d %H:%M:%S") << "\n";
}

std::string SaveManager::readSaveTimestamp(const std::string &filename)
{
    std::ifstream inFile(filename);
    if (!inFile.is_open())
        throw std::runtime_error("Failed to open save file for reading timestamp");

    std::string line;
    while (std::getline(inFile, line))
    {
        if (line.find("SaveDateTime: ") == 0) // اگر خط با این رشته شروع شد
        {
            // جدا کردن قسمت تاریخ و ساعت
            return line.substr(std::string("SaveDateTime: ").length());
        }
    }

    // اگر تاریخ پیدا نشد، رشته خالی برگردون یا خطا بده
    return "";
}

std::vector<std::pair<int, std::string>> SaveManager::getAllSaveSlotsWithTimestamps()
{
    std::vector<std::pair<int, std::string>> result;

    int i = 1;
    while (true)
    {
        std::string filename = "save" + std::to_string(i) + ".txt";
        std::ifstream file(filename);
        if (!file.good())
            break; // اگر فایل وجود نداشت، تموم کنیم

        std::string timestamp = readSaveTimestamp(filename);
        result.push_back({i, timestamp});
        ++i;
    }

    return result;
}

void SaveManager::saveSceneToFile(std::string &filename, std::string scenekey)
{
    std::ofstream outFile(filename, std::ios::app); // append
    if (!outFile.is_open())
        throw GameException("Cannot open file to save current scene state!");

    outFile << "Scene|" << scenekey << "\n";

    if (scenekey == "ItemSelectionScene")
    {
        auto &ItemScene = SceneManager::getInstance().getScene<ItemSelectionScene>(SceneKeys::ITEM_SELECTION_SCENE);
        outFile << "SceneData|ItemSelectionScene\n";
        outFile << "ReturnScene|" << ItemScene.getscenekey() << "\n";
        for (Item *item : ItemScene.getItems())
            outFile << item->serialize() << "\n";
    }
    else if (scenekey == "VillagerSelectionScene")
    {
        auto &VillScene = SceneManager::getInstance().getScene<VillagerSelectionScene>(SceneKeys::VILLAGER_SELECTION_SCENE);
        outFile << "SceneData|VillagerSelectionScene\n";
        outFile << "ReturnScene|" << VillScene.getscenekey() << "\n";
        for (Villager *vill : VillScene.getVillagers())
            outFile << "Villager|" << vill->serialize() << "\n";
    }
    else if (scenekey == "MonsterSelectionScene")
    {
        auto &monScene = SceneManager::getInstance().getScene<MonsterSelectionScene>(SceneKeys::MONSTER_SELECTION_SCENE);
        outFile << "SceneData|MonsterSelectionScene\n";
        outFile << "ReturnScene|" << monScene.getscenekey() << "\n";
        for (Monster *mon : monScene.getMonsters())
            outFile << "Monster|" << mon->serialize() << "\n";
    }
    else if (scenekey == "PerkSelectionScene")
    {
        auto &perkScene = SceneManager::getInstance().getScene<PerkSelectionScene>(SceneKeys::PERK_SELECTION_SCENE);
        outFile << "SceneData|PerkSelectionScene\n";
        outFile << "ReturnScene|" << perkScene.getscenekey() << "\n";
        for (PerkCard perk : perkScene.getperks())
            outFile << perk.serialize() << "\n";
    }
    else if (scenekey == "PlayerSummaryScene")
    {
        auto &sumScene = SceneManager::getInstance().getScene<PlayerSummaryScene>(SceneKeys::PLAYER_SUMMARY_SCENE);
        outFile << "SceneData|PlayerSummaryScene\n";
        for (std::string str : sumScene.getData())
            outFile << "Data|" << str << "\n";
    }
    else if (scenekey == "LocationSelectionScene")
    {
        auto &locScene = SceneManager::getInstance().getScene<LocationSelectionScene>(SceneKeys::LOCATION_SELECTION_SCENE);
        outFile << "SceneData|LocationSelectionScene\n";
        outFile << "ReturnScene|" << locScene.getscenekey() << "\n";
        for (Location *loc : locScene.getSentlocations())
            outFile << loc->serialize() << "\n";
    }

    outFile << "EndScene\n";
}

void SaveManager::loadSceneFromFile(const std::string &filename)
{
    std::ifstream inFile(filename);
    if (!inFile.is_open())
        throw GameException("Cannot open file to load scene!");

    std::string line, scenekey, subSceneKey;

    // Scene key
    while (std::getline(inFile, line)) {
        if (line.rfind("Scene|", 0) == 0) {
            scenekey = line.substr(6);
            break;
        }
    }

    if (scenekey.empty())
        throw GameException("No scene key found in save file!");

    //ReturnScene
    while (std::getline(inFile, line)) {
        if (line.rfind("SceneData|", 0) == 0) {
            continue; // نادیده بگیر
        }
        else if (line.rfind("ReturnScene|", 0) == 0) {
            subSceneKey = line.substr(12);
            continue; 
        }
        else {
            break; // Data|
        }
    }

    //بازیابی دیتا برای هر صحنه
    if (scenekey == "ItemSelectionScene") {
        auto &scene = SceneManager::getInstance().getScene<ItemSelectionScene>(SceneKeys::ITEM_SELECTION_SCENE);
        std::vector<Item *> items;

        do {
            if (line == "EndScene") break;
            if (line.rfind("Item|", 0) == 0) {
                Item *item = Item::deserialize(line.substr(5));
                if (item) items.push_back(item);
            }
        } while (std::getline(inFile, line));

        scene.setData(items, subSceneKey);
    }
    else if (scenekey == "VillagerSelectionScene") {
        auto &scene = SceneManager::getInstance().getScene<VillagerSelectionScene>(SceneKeys::VILLAGER_SELECTION_SCENE);
        std::vector<Villager *> villagers;

        do {
            if (line == "EndScene") break;
            if (line.rfind("Villager|", 0) == 0) {
                Villager *v = Villager::deserialize(line.substr(9));
                if (v) villagers.push_back(v);
            }
        } while (std::getline(inFile, line));

        scene.setData(villagers, subSceneKey);
    }
    else if (scenekey == "MonsterSelectionScene") {
        auto &scene = SceneManager::getInstance().getScene<MonsterSelectionScene>(SceneKeys::MONSTER_SELECTION_SCENE);
        std::vector<Monster *> monsters;

        do {
            if (line == "EndScene") break;
            if (line.rfind("Monster|", 0) == 0) {
                Monster *m = Monster::deserialize(line.substr(8));
                if (m) monsters.push_back(m);
            }
        } while (std::getline(inFile, line));

        scene.setDate(monsters, subSceneKey);
    }
    else if (scenekey == "PerkSelectionScene") {
        auto &scene = SceneManager::getInstance().getScene<PerkSelectionScene>(SceneKeys::PERK_SELECTION_SCENE);
        std::vector<PerkCard> perks;

        do {
            if (line == "EndScene") break;
            if (line.rfind("PerkCard|", 0) == 0) {
                perks.push_back(PerkCard::deserialize(line.substr(5)));
            }
        } while (std::getline(inFile, line));

        scene.setData(perks, subSceneKey);
    }
    else if (scenekey == "PlayerSummaryScene") {
        auto &scene = SceneManager::getInstance().getScene<PlayerSummaryScene>(SceneKeys::PLAYER_SUMMARY_SCENE);
        std::vector<std::string> summary;

        do {
            if (line == "EndScene") break;
            if (line.rfind("Data|", 0) == 0)
                summary.push_back(line.substr(5));
        } while (std::getline(inFile, line));

        if (summary.size() != 4)
            throw GameException("Corrupted save file: incomplete summary data");

        scene.setData(summary[0], summary[1], summary[2], summary[3]);
    }
    else if (scenekey == "LocationSelectionScene") {
        auto &scene = SceneManager::getInstance().getScene<LocationSelectionScene>(SceneKeys::LOCATION_SELECTION_SCENE);
        std::vector<Location *> locations;

        do {
            if (line == "EndScene") break;
            if (line.empty()) continue;

            Location *loc = Location::deserialize(line);
            if (loc) locations.push_back(loc);
        } while (std::getline(inFile, line));

        scene.setData(locations, subSceneKey);
    }

    SceneManager::getInstance().goTo(scenekey);
}
