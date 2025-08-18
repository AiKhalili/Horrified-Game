#ifndef SAVEMANAGER_HPP
#define SAVEMANAGER_HPP
#include <string>
#include <vector>
#include <ctime>
#include <iomanip>
#include <utility>
#include "core/Hero.hpp"
#include "core/Mayor.hpp"
#include "core/Archaeologist.hpp"
#include "core/PerkCard.hpp"
#include "core/Monster.hpp"
#include "core/Dracula.hpp"
#include "core/Invisible_Man.hpp"
#include "core/MonsterCard.hpp"
#include "core/Villager.hpp"
#include "core/Location.hpp"
#include "core/Map.hpp"
#include "core/Item.hpp"
#include "core/GameException.hpp"

class Game;
class Map;

class SaveManager
{
private:
    SaveManager() = default;

    SaveManager(const SaveManager &) = delete;
    SaveManager &operator=(const SaveManager &) = delete;

    std::vector<Item *> ItemInter;

public:
    static SaveManager &getInstance();

    std::vector<Item *>& getItemInter();

    void saveGameToSlot(std::string newkey); // Full game save according to the scene we saved
    void loadGameFromSlot(int);              // Load the game with the slot number that the user entered and wants to continue the game

    void saveHeroesToFile(const std::string &, const std::vector<Hero *> &); // Save the heros in the game
    std::vector<Hero *> loadHeroesFromFile(const std::string &);             // Reading and loading information for each hero

    void saveCurrentHeroStateToFile(const std::string &, int, int);        // Save the active hero in the game
    std::pair<int, int> loadCurrentHeroStateFromFile(const std::string &); // Reading hero information and building it based on previous information to continue the game

    void saveMonstersToFile(const std::string &, const std::vector<Monster *> &); // Save each monster and its information
    std::vector<Monster *> loadMonstersFromFile(const std::string &);             // Reading each monster's information and loading it

    void saveVillagersToFile(const std::string &, const std::vector<Villager *> &); // Save the villagers in the game and information about them
    std::vector<Villager *> loadVillagersFromFile(const std::string &);             // Reading each villager's information and loading it

    void saveItemsToFile(const std::string &, const std::vector<Item *> &); // Save the items in the game and information about them
    std::vector<Item *> loadItemsFromFile(const std::string &);             // Reading each items's information and loading it

    void saveMonsterCardsToFile(const std::string &, const std::vector<MonsterCard> &); // Save the monster cards and information about them
    std::vector<MonsterCard> loadMonsterCardsFromFile(const std::string &);             // Load the monster cards

    void saveItemManagerStateToFile(const std::string &);   // Save the current state of the ItemManager to a file, This includes the bag contents and used items
    void loadItemManagerStateFromFile(const std::string &); // Load the ItemManager state from a file, Reads bag contents and used items

    void saveTerrorLevel(const std::string &, int); // Save the Terror Level at that stage of the game
    int loadTerrorLevel(const std::string &);       // Loading horror levels during gameplay

    void saveFrenzy(const std::string &, const std::string &);                // Save the current Frenzy monster's name to a file
    Monster *loadFrenzy(const std::string &, const std::vector<Monster *> &); // Load the Frenzy monster from a file

    void savePerkDeckToFile(const std::string &, const std::vector<PerkCard> &); // Save the perk cards and information about them
    std::vector<PerkCard> loadPerkDeckFromFile(const std::string &);             // Load the perk cards

    void writeSaveTimestamp(const std::string &);       // Save game save date and time
    std::string readSaveTimestamp(const std::string &); // Load game save date and time

    std::vector<std::pair<int, std::string>> getAllSaveSlotsWithTimestamps(); // Retrieve all available save slots along with their timestamps

    void saveAllScenesToFile(const std::string &filename,const std::string &scenekey); // Save all the scene information from where we saved the game
    void loadAllScenesFromFile(const std::string &);                       // Loading all the scene information from where we saved the game to continue the game from there
};
#endif
