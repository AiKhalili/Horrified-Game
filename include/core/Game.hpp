#ifndef GAME_HPP
#define GAME_HPP

#include <vector>
#include <string>
#include <iomanip>
#include <cstdio>
#include "core/Map.hpp"
#include "core/Hero.hpp"
#include "core/Archaeologist.hpp"
#include "core/Mayor.hpp"
#include "core/Scientist.hpp"
#include "core/Courier.hpp"
#include "core/Monster.hpp"
#include "core/Villager.hpp"
#include "core/Item.hpp"
#include "core/Dracula.hpp"
#include "core/Invisible_Man.hpp"
#include "core/ItemManager.hpp"
#include "core/PerkCard.hpp"
#include "core/MonsterCard.hpp"
#include "core/GameException.hpp"
#include <utility>
#include <map>

class MonsterPhaseScene;
class MoveScene;
class AdvanceScene;
class PickUpScene;
class SpecialScene;
class GuidScene;
class HelpScene;
class PerkSelection;
class DefeatScene;

enum class GameState
{
    None,

    HeroPhase,
    EndHeroPhase,

    ShowPerkCard,

    StartMonsterPhase,
    EndMonsterPhase,

    GameWon,
    GameLost,
    GameOutOfTime
};

enum class StrikeResult
{
    HeroAndVillagerNotFound,
    HeroFound,
    VillagerFound
};

struct MonsterCardEvent
{
    std::string msg;
    std::string heroName;
    std::string monsterName;
    std::string locationName;
    std::string villagerName;
    std::vector<std::pair<std::string, std::string>> villagersAndLocations;
};

class Game
{
    friend class MonsterPhaseScene;
    friend class MoveScene;
    friend class AdvanceScene;
    friend class PickUpScene;
    friend class SpecialScene;
    friend class GuidScene;
    friend class HelpScene;
    friend class PerkSelectionScene;
    friend class DefeatScene;

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
    GameState currentState = GameState::None;
    PerkCard rewardedPerkCard;
    Villager *rescuedVillager = nullptr;

    PLayerData player1, player2;
    int player1_time = 0, player2_time = 0; // زمان سیر خوردن

    int currentHeroIndex;              // نوبت فعلی (0 یا 1)
    int terrorLevel;                   // میزان ترس
    int maxTerror;                     // حداکثر ترس مجاز
    bool skipNextMonsterPhase = false; // رد کردن فاز هیولای بعدی

    MonsterCardEvent event;

    int Slot = 0;
    int currentSaveSlot = 1;
    bool isLoadedFromFile = false;
    bool firstHeroPhaseAfterLoad = false;

    // مقدار دهی اولیه
    void setup();
    void setupMonsters();
    void setupItems();
    void setupPerkCards();
    void setupMonsterCards();

    void rescueVillagerIfSafe(Hero *); // بررسی نجات یافتن محلی ها
    PerkCard drawPerkCard();           // کشیدن یک پرک کارت

    void useMonsterCard(const std::string &);

    void nextTurn();

    // بررسی شرایط پایان
    bool checkWinCondition() const;
    bool checkLoseCondition() const;
    bool checkOutOfTime() const;

    void updateHeroPhase();
    void updateCheckEndOfGame();
    void updateEndHeroPhase();

    std::vector<std::string> MonstersStrike;
    MonsterCard currentMosnterCard;
    int currentStrikeIndex = 0;
    std::map<Face, int> diceCount;
    Monster *targetMonster = nullptr;

    void setupMonsterStrike();

    void sendHeroToHospital();
    void defendHero(Item *);

    Hero *damageHero = nullptr;
    Villager *damageVillager = nullptr;

    bool canStartMonsterPhase();
    void drawMonsterCard();
    std::vector<Item *> placeItemsOnMap();
    bool moveMonster();
    std::vector<std::string> rollingDice();
    StrikeResult diceStrikeFace();

    void handleMove(Location *loc, std::vector<Villager *> vills);
    void handlePickUP(std::vector<Item *> items);
    void handleGuide(std::vector<Villager *> v, Location *dest);
    void handleAdvance(Monster *mon, std::vector<Item *> items);
    void handleDefeat(Monster *mon, std::vector<Item *> items);
    void handleSpecialAction(Location *selectedLocation, const std::vector<Item *> &selectedItems);
    void usePerkCard(int, std::vector<Location *>);

public:
    static Game &getInstance();

    // توابع set
    void setPlayersTimes(int, const std::string &, const std::string &);
    void setStartingPlater(const std::string &);
    void setGameState(GameState state);

    // توابع get
    std::string getName1() const;
    std::string getName2() const;
    int getTime1() const;
    int getTime2() const;
    Hero *getCurrentHero();
    Monster *getFrenzy() const;
    std::vector<Hero *> getAllHeroes() const;
    GameState getGameState() const;
    Villager *getLastRescuedVillager() const;
    PerkCard getLastRewardedPerkCard() const;

    // شروع بازی
    void assignHeroes(const std::string &, const std::string &, const std::string &);
    void startNewGame();
    void startLoadedGame(int);

    void setupHeroes();

    void updateFrenzy(); // تغییر نشان فرنزی

    void update();

    void reset();

    void setSlot(int);
    int findNextFreeSlot() const;
    void setCurrentSaveSlot(int slot);
    int getCurrentSaveSlot() const;
    void setLoadedFromFile(bool b);
    bool getLoadedFromFile() const;

    std::vector<Hero *> getHeroes() const;
    std::vector<Monster *> getMonsters() const;
    std::vector<Villager *> getVillagers() const;
    std::vector<PerkCard> getPerkDeck() const;
    std::vector<MonsterCard> getMonsterDeck() const;

    int getSlot() const;
    int getTerrorLevel() const;
    int getCurrentHeroIndex() const;
    int getPlayer1Time() const;
    int getPlayer2Time() const;
    bool getSkipNextMonsterPhase() const;
    Monster *getFrenzyMonster() const;

    void setHeroes(const std::vector<Hero *> &);
    void setMonsters(const std::vector<Monster *> &);
    void setVillagers(const std::vector<Villager *> &);
    void setPerkDeck(const std::vector<PerkCard> &);
    void setMonsterDeck(const std::vector<MonsterCard> &);

    void setTerrorLevel(int);
    void setCurrentHeroIndex(int);
    void setPlayerTimes(int time1, int time2);
    void setSkipNextMonsterPhase(bool);
    void setFrenzyMonster(Monster *);

    void handleGameOver();
    void prepareForSaving();
    bool shouldShowSpecialAcion();
};

#endif