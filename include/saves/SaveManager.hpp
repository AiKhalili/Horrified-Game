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

public:

    void saveGameToSlot(int, Game&); // ذخیره کامل بازی با شماره اسلات
    void loadGameFromSlot(int, Game&); // بارگذاری بازی با شماره اسلاتی که کاربر وارد کرده و میخواد بازی رو ادامه بده

    void saveHeroesToFile(const std::string&, const std::vector<Hero*>& ); // ذخیره کردن قهرمان فعال بازی
    std::vector<Hero*> loadHeroesFromFile(const std::string&); // خواندن اطلاعات قهرمان و ساختن آن بر اساس اطاعات قبلی برای ادامه بازی

    void saveCurrentHeroStateToFile(const std::string& , int, int);
    std::pair<int, int> loadCurrentHeroStateFromFile(const std::string&);
    
    void saveMonstersToFile(const std::string&, const std::vector<Monster*>&);
    std::vector<Monster*> loadMonstersFromFile(const std::string&);

    void saveVillagersToFile(const std::string&, const std::vector<Villager*>&);
    std::vector<Villager*> loadVillagersFromFile(const std::string&);

    void saveItemsToFile(const std::string&, const std::vector<Item*>&);
    std::vector<Item*> loadItemsFromFile(const std::string&);

    void saveMonsterCardsToFile(const std::string& , const std::vector<MonsterCard>&);
    std::vector<MonsterCard> loadMonsterCardsFromFile(const std::string&);

    void saveItemManagerStateToFile(const std::string&);
    void loadItemManagerStateFromFile(const std::string&);

    void saveTerrorLevel(const std::string&, int);
    int loadTerrorLevel(const std::string&);

    void saveCurrentTurn(const std::string&, int);
    int loadCurrentTurn(const std::string&);

    void saveTimes(const std::string&, int, int);
    void loadTimes(const std::string&, int&, int&);

    void saveFrenzy(const std::string& , const std::string&);
    Monster* loadFrenzy(const std::string&, const std::vector<Monster*>&);

    void savePerkDeckToFile(const std::string&, const std::vector<PerkCard>&);
    std::vector<PerkCard> loadPerkDeckFromFile(const std::string&);

    void writeSaveTimestamp(const std::string&);
    std::string readSaveTimestamp(const std::string& );

    std::vector<std::pair<int, std::string>> getAllSaveSlotsWithTimestamps();

};

#endif