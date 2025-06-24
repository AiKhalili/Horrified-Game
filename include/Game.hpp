#ifndef GAME_HPP
#define GAME_HPP

#include <vector>
#include <string>
#include <iomanip>
#include "Map.hpp"
#include "Hero.hpp"
#include "Archaeologist.hpp"
#include "Mayor.hpp"
#include "Monster.hpp"
#include "Villager.hpp"
#include "Item.hpp"
#include "Dracula.hpp"
#include "Invisible_Man.hpp"
#include "ItemManager.hpp"
#include "PerkCard.hpp"
#include "MonsterCard.hpp"
#include "GameException.hpp"

class Game
{
private:
    Game();
    ~Game();

    Game(const Game &) = delete;
    Game &operator=(const Game &) = delete;

    struct PLayerData
    {
        std::string name;
        std::string heroClass;
    };

    Map *map;
    Monster *frenzy = nullptr;            // هیولای فرنزی رو نگه میداره
    std::vector<Hero *> heroes;           // دو قهرمان
    std::vector<Monster *> monsters;      // دو هیولا
    std::vector<Villager *> villagers;    // لیست تمام Villagerها
    std::vector<PerkCard> perkDeck;       // لیست کارت‌های پرک
    std::vector<MonsterCard> monsterDeck; // لیست کارت‌های هیولا

    PLayerData player1, player2;
    int player1_time = 0, player2_time = 0; // زمان سیر خوردن

    int currentHeroIndex;              // نوبت فعلی (0 یا 1)
    int terrorLevel;                   // میزان ترس
    int maxTerror;                     // حداکثر ترس مجاز
    bool skipNextMonsterPhase = false; // رد کردن فاز هیولای بعدی

    // مقدار دهی اولیه
    void setup();
    void setupMonsters();
    void setupItems();
    void setupPerkCards();
    void setupMonsterCards();

    void rescueVillagerIfSafe(Hero *); // بررسی نجات یافتن محلی ها
    PerkCard drawPerkCard();           // کشیدن یک پرک کارت

    // اجرای کارت های بازی
    void usePerkCard();
    void useMonsterCard(const std::string &);

    void heroPhase(Hero *hero);
    void monsterPhase();

    void nextTurn();

    // بررسی شرایط پایان
    bool checkWinCondition() const;
    bool checkLoseCondition() const;
    bool checkOutOfTime() const;

    // انتخاب های بازیکنان برای ادامه بازی
    Location *askLocationChoice(const std::vector<Location *> &);
    std::vector<Villager *> askVillagerChoice(const std::vector<Villager *> &);
    std::vector<Item *> askItemSelection(const std::vector<Item *> &);
    Monster *askMonsterChoice(const std::vector<Monster *> &);
    int askPerkCardChoice(Hero *);
    Hero *askHeroChoice();
    Item *askItemToDefend(const std::vector<Item *> &);

    // توابع نمایش منو
    std::vector<std::string> getHeroLines(const std::vector<Hero *> &);
    std::vector<std::string> getVillagerLines(const std::vector<Villager *> &);
    std::vector<std::string> getMonsterLines(const std::vector<Monster *> &);
    std::vector<std::string> getItemLines(const std::vector<Item *> &);
    void showGameStatuse(); // نمایش وضعیت بازی
    std::string actionMenu();
    void info();
    void boundary() const;

public:
    static Game &getInstance();

    // توابع set
    void setPlayersTimes(int, const std::string &, const std::string &);
    void setStartingPlater(const std::string &);

    // توابع get
    std::string getName1() const;
    std::string getName2() const;
    int getTime1() const;
    int getTime2() const;
    Hero *getCurrentHero();
    Monster *getFrenzy() const;

    // شروع بازی
    void assignHeroes(const std::string &, const std::string &, const std::string &);
    void start();
    void setupHeroes();

    void updateFrenzy(); // تغییر نشان فرنزی

    void reset();
};

#endif