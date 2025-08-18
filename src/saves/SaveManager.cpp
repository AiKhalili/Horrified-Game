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

std::vector<Item *> &SaveManager::getItemInter() { return ItemInter; }

void SaveManager::saveGameToSlot(std::string newkey)
{
    int slotNumber;

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

    std::ofstream outFile(filename, std::ios::out | std::ios::trunc); // قبلی پاک می‌شود
    if (!outFile.is_open())
        return;

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

    saveAllScenesToFile(filename, newkey);
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
        {
            item->get_location()->addItem(item);
            ItemInter.push_back(item);
        }
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

    loadAllScenesFromFile(filename);
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
            std::string name = line.substr(7);
            for (Monster *m : monsters)
            {
                if (m && m->get_name() == name)
                    return m;
            }
            throw GameException("Monster with name '" + name + "' not found to set as Frenzy!");
        }
    }

    return nullptr; 
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

static const std::vector<std::string> allSceneKeys = {

    SceneKeys::PLAYER_SUMMARY_SCENE,
    SceneKeys::MONSTERS_SUMMARY_SCENE,
    SceneKeys::BOARD_SCENE,
    SceneKeys::CHEST_INFO_SCENE,
    SceneKeys::PERK_CARD_SCENE,
    SceneKeys::LOCATION_INFO_SCENE,
    SceneKeys::SHOW_ACTIONS_SCENE,
    SceneKeys::ITEM_SELECTION_SCENE,
    SceneKeys::MONSTER_SELECTION_SCENE,
    SceneKeys::VILLAGER_SELECTION_SCENE,
    SceneKeys::LOCATION_SELECTION_SCENE,
    SceneKeys::PERK_SELECTION_SCENE,
    SceneKeys::MOVE_SCENE,
    SceneKeys::PICK_UP_SCENE,
    SceneKeys::GUID_SCENE,
    SceneKeys::ADVANCE_SCENE,
    SceneKeys::DEFEAT_SCENE,
    SceneKeys::SPECIAL_SCENE,
    SceneKeys::HELP_SCENE,
    SceneKeys::RESCUE_RESULT_SCENE,
    SceneKeys::END_GAME_SCENE,
};

void SaveManager::saveAllScenesToFile(const std::string &filename, const std::string &currentSceneKey)
{
    std::ofstream outFile(filename, std::ios::app);
    if (!outFile.is_open())
        return;

    outFile << "CurrentScene:" << currentSceneKey << "\n";

    std::vector<std::string> scenesToSave;

    if (currentSceneKey == SceneKeys::PLAYER_SUMMARY_SCENE)
    {
        scenesToSave.push_back(SceneKeys::PLAYER_SUMMARY_SCENE);
    }
    else if (currentSceneKey == SceneKeys::MONSTERS_SUMMARY_SCENE)
    {
        scenesToSave.push_back(SceneKeys::PLAYER_SUMMARY_SCENE);
        scenesToSave.push_back(SceneKeys::MONSTERS_SUMMARY_SCENE);
    }
    else
    {
        scenesToSave = allSceneKeys;
    }

    scenesToSave.erase(std::remove(scenesToSave.begin(), scenesToSave.end(), currentSceneKey), scenesToSave.end());

    for (const auto &key : scenesToSave)
    {
        Scene *scene = &SceneManager::getInstance().getScene<Scene>(key);
        if (scene)
            scene->serialize(filename);
    }

    Scene *currentScene = &SceneManager::getInstance().getScene<Scene>(currentSceneKey);
    if (currentScene)
        currentScene->serialize(filename);

    outFile.close();
}

void SaveManager::loadAllScenesFromFile(const std::string &filename)
{
    std::ifstream inFile(filename);
    if (!inFile.is_open())
        return;

    std::string line;
    std::string currentSceneKey;
    std::vector<std::string> scenesToLoad;

    while (std::getline(inFile, line))
    {
        if (line.find("CurrentScene:") == 0)
        {
            currentSceneKey = line.substr(std::string("CurrentScene:").length());
            break;
        }
    }

    if (currentSceneKey == SceneKeys::PLAYER_SUMMARY_SCENE)
    {
        scenesToLoad.push_back(SceneKeys::PLAYER_SUMMARY_SCENE);
    }
    else if (currentSceneKey == SceneKeys::MONSTERS_SUMMARY_SCENE)
    {
        scenesToLoad.push_back(SceneKeys::PLAYER_SUMMARY_SCENE);
        scenesToLoad.push_back(SceneKeys::MONSTERS_SUMMARY_SCENE);
    }
    else
    {
        scenesToLoad = allSceneKeys;
    }

    inFile.clear();
    inFile.seekg(0);

    for (const auto &key : scenesToLoad)
    {
        if (key == currentSceneKey)
            continue;
        Scene *scene = &SceneManager::getInstance().getScene<Scene>(key);
        if (scene)
        {
            scene->deserialize(filename);
        }
    }

    if (!currentSceneKey.empty())
    {
        Scene *currentScene = &SceneManager::getInstance().getScene<Scene>(currentSceneKey);
        if (currentScene)
            currentScene->deserialize(filename);

        SceneManager::getInstance().goTo(currentSceneKey);
    }

    inFile.close();
}
