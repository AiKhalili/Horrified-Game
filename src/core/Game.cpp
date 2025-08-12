#include "core/Game.hpp"
#include <algorithm>
#include <random>
#include <cstdlib>
#include <stdexcept>
#include <saves/SaveManager.hpp>
#include <fstream>
#include <unordered_map>

using namespace std;

Game::Game() : map(Map::get_instanse()) {}

void Game::setPlayersTimes(int playerNumber, const string &name, const string &time)
{
    if (name.empty())
    {
        throw invalid_argument("Name cannot be empty.");
    }

    if ((playerNumber == 1 && name == player2.name) ||
        (playerNumber == 2 && name == player1.name))
    {
        throw invalid_argument("Duplicate player names are not allowed.\n");
    }

    if (time.empty())
    {
        throw invalid_argument("Time must contain digits only.\n");
    }

    // بررسی کاراکترهای مجاز: فقط عدد یا عدد با علامت منفی
    size_t start = (time[0] == '-') ? 1 : 0;
    if (start == 1 && time.size() == 1) // یعنی فقط "-"
    {
        throw invalid_argument("Time must contain digits only.\n");
    }

    for (size_t i = start; i < time.size(); ++i)
    {
        if (!isdigit(time[i]))
        {
            throw invalid_argument("Time must contain digits only.\n");
        }
    }

    int parsed_time = stoi(time);
    if (parsed_time < 0)
    {
        throw invalid_argument("Time cannot be a negative number.\n");
    }

    if (playerNumber == 1)
    {
        player1.name = name;
        player1_time = parsed_time;
    }
    else if (playerNumber == 2)
    {
        player2.name = name;
        player2_time = parsed_time;
    }
    else
    {
        throw invalid_argument("Invalid player number.\n");
    }
}

string Game::getName1() const
{
    return player1.name;
}

string Game::getName2() const
{
    return player2.name;
}

int Game::getTime1() const
{
    return player1_time;
}

int Game::getTime2() const
{
    return player2_time;
}

int Game::getSlot() const { return Slot; }

Game &Game::getInstance()
{
    static Game instance;
    return instance;
}

void Game::assignHeroes(const std::string &name1, const std::string &hero1, const std::string &hero2)
{
    if (hero1.empty() || hero2.empty())
    {
        throw GameException("Hero class cannot be empty!");
    }
    if (player1.name == name1)
    {
        player1.heroClass = hero1;
        player2.heroClass = hero2;
    }
    else
    {
        player1.heroClass = hero2;
        player2.heroClass = hero1;
    }
}

void Game::setStartingPlater(const string &starterName)
{
    if (heroes.size() < 2)
    {
        throw GameException("Not enough hero initialized!");
    }
    if (player1.name == starterName)
    {
        currentHeroIndex = 0;
    }
    else if (player2.name == starterName)
    {
        currentHeroIndex = 1;
    }
    else
    {
        throw GameException("Invalid starter name!");
    }
}

Villager *Game::getLastRescuedVillager() const
{
    return rescuedVillager;
}

PerkCard Game::getLastRewardedPerkCard() const
{
    return rewardedPerkCard;
}

void Game::updateHeroPhase()
{
    Hero *hero = getCurrentHero();
    if (!hero)
    {
        return;
    }

    updateCheckEndOfGame();

    if (currentState != GameState::HeroPhase)
    {
        return;
    }

    rescueVillagerIfSafe(hero);

    if (currentState != GameState::HeroPhase)
    {
        return;
    }
    if (hero->getActionsLeft() <= 0)
    {
        currentState = GameState::EndHeroPhase;
    }
}

void Game::updateCheckEndOfGame()
{
    if (checkWinCondition())
    {
        currentState = GameState::GameWon;
        return;
    }
    if (checkLoseCondition())
    {
        currentState = GameState::GameLost;
        return;
    }
    if (checkOutOfTime())
    {
        currentState = GameState::GameOutOfTime;
        return;
    }
}

void Game::updateEndHeroPhase()
{
    Hero *hero = getCurrentHero();
    if (hero)
    {
        getCurrentHero()->resetActions();
    }
    currentState = GameState::StartMonsterPhase;
}

void Game::update()
{
    switch (currentState)
    {
    case GameState::HeroPhase:
        updateHeroPhase();
        break;
    case GameState::EndHeroPhase:
        updateEndHeroPhase();
        break;
    case GameState::EndMonsterPhase:
        diceCount.clear();
        updateCheckEndOfGame();
        nextTurn();
        currentState = GameState::HeroPhase;
        break;
    case GameState::ShowPerkCard:
        currentState = GameState::HeroPhase;
    default:
        break;
    }
}

void Game::startNewGame()
{
    setup(); // آماده‌سازی اولیه
    setGameState(GameState::HeroPhase);
}

void Game::startLoadedGame(int slot)
{
    SaveManager::getInstance().loadGameFromSlot(slot); // فایل اسلات رو می‌خونه و مقداردهی می‌کنه
    setGameState(GameState::HeroPhase);
}

GameState Game::getGameState() const
{
    return currentState;
}

void Game::setGameState(GameState state)
{
    currentState = state;
}

void Game::setup()
{
    setupMonsters();
    setupItems();
    setupPerkCards();
    setupMonsterCards();
    terrorLevel = 0;
    maxTerror = 5;
}

void Game::setupHeroes()
{
    if (!map)
    {
        map = Map::get_instanse();
    }
    heroes.clear();

    Hero *hero1 = nullptr;
    Hero *hero2 = nullptr;

    if (player1.heroClass == "Mayor")
    {
        hero1 = new Mayor(player1.name, map);
    }
    else if (player1.heroClass == "Archaeologist")
    {
        hero1 = new Archaeologist(player1.name, map);
    }
    else if (player1.heroClass == "Courier")
    {
        hero1 = new Courier(player1.name, map);
    }
    else if (player1.heroClass == "Scientist")
    {
        hero1 = new Scientist(player1.name, map);
    }
    else
    {
        throw GameException("Invalid hero class for Player 1!");
    }

    if (player2.heroClass == "Mayor")
    {
        hero2 = new Mayor(player2.name, map);
    }
    else if (player2.heroClass == "Archaeologist")
    {
        hero2 = new Archaeologist(player2.name, map);
    }

    else if (player2.heroClass == "Courier")
    {
        hero2 = new Courier(player2.name, map);
    }
    else if (player2.heroClass == "Scientist")
    {
        hero2 = new Scientist(player2.name, map);
    }
    else
    {
        throw GameException("Invalid hero class for Player 2!");
    }

    if (currentHeroIndex == 0)
    {
        heroes.push_back(hero1);
        heroes.push_back(hero2);
    }
    else
    {
        heroes.push_back(hero2);
        heroes.push_back(hero1);
    }
}

void Game::setupMonsters()
{
    monsters.clear();

    Monster *dracula = new Dracula();
    Monster *invisibleMan = new Invisible_Man();

    monsters.push_back(dracula);
    monsters.push_back(invisibleMan);

    sort(monsters.begin(), monsters.end(), [](Monster *a, Monster *b)
         { return a->getFrenzyOrder() < b->getFrenzyOrder(); });
    frenzy = monsters.front();
}

void Game::setupItems()
{
    ItemManager &manager = ItemManager::getInstance();

    manager.initializeItems(); // ساخت ۶۰ آیتم و گذاشتن تو bag

    // اضافه کردن ۱۲ آیتم تصادفی برای شروع بازی
    int added = 0;

    while (added < 12 && manager.ramainingItems() > 0)
    {
        Item *item = manager.getRandomItem();
        if (!item)
        {
            break;
        }

        Location *loc = item->get_location();
        if (loc)
        {
            loc->addItem(item);
            added++;
        }
    }
}

void Game::setupPerkCards()
{

    for (int i = 0; i < 3; ++i)
    { // ساخت کارت‌ها طبق تعداد مشخص
        perkDeck.emplace_back(PerkType::VISIT_FROM_THE_DETECTIVE);
        perkDeck.emplace_back(PerkType::BREAK_OF_DAWN);
         perkDeck.emplace_back(PerkType::REPEL);
        perkDeck.emplace_back(PerkType::HURRY);
    }

    for (int i = 0; i < 4; ++i)
    {
        perkDeck.emplace_back(PerkType::OVERSTOCK);
        perkDeck.emplace_back(PerkType::LATE_INTO_THE_NIGHT);
    }

    // بر زدن کارت‌ها
    random_device rd;
    mt19937 g(rd());
    shuffle(perkDeck.begin(), perkDeck.end(), g);

    // دادن ۱ کارت به هر قهرمان
    for (int i = 0; i < 2; ++i)
    {
        Hero *h = heroes[i];
        if (!perkDeck.empty())
        {
            h->setPerkCard(perkDeck.back());
            perkDeck.pop_back();
        }
    }
}

void Game::setupMonsterCards()
{
    monsterDeck.clear();

    monsterDeck.push_back(MonsterCard("FormOfTheBat", 2, "Place Dracula in the space with the current Player's Hero.", 1, 2, {"InvisibleMan"}));
    monsterDeck.push_back(MonsterCard("FormOfTheBat", 2, "Place Dracula in the space with the current Player's Hero.", 1, 2, {"InvisibleMan"}));
    monsterDeck.push_back(MonsterCard("FormOfTheBat", 2, "Place Dracula in the space with the current Player's Hero.", 1, 2, {"InvisibleMan"}));

    monsterDeck.push_back(MonsterCard("Sunrise", 0, "Place Dracula at the crypt.", 1, 2, {"InvisibleMan", "Frenzied Monster"}));
    monsterDeck.push_back(MonsterCard("Sunrise", 0, "Place Dracula at the crypt.", 1, 2, {"InvisibleMan", "Frenzied Monster"}));
    monsterDeck.push_back(MonsterCard("Sunrise", 0, "Place Dracula at the crypt.", 1, 2, {"InvisibleMan", "Frenzied Monster"}));

    monsterDeck.push_back(MonsterCard("Thief", 2, "Place the Invisible Man at the location with the most Items.\nDiscard all Items there.", 1, 3, {"InvisibleMan", "Dracula"}));
    monsterDeck.push_back(MonsterCard("Thief", 2, "Place the Invisible Man at the location with the most Items.\nDiscard all Items there.", 1, 3, {"InvisibleMan", "Dracula"}));
    monsterDeck.push_back(MonsterCard("Thief", 2, "Place the Invisible Man at the location with the most Items.\nDiscard all Items there.", 1, 3, {"InvisibleMan", "Dracula"}));
    monsterDeck.push_back(MonsterCard("Thief", 2, "Place the Invisible Man at the location with the most Items.\nDiscard all Items there.", 1, 3, {"InvisibleMan", "Dracula"}));
    monsterDeck.push_back(MonsterCard("Thief", 2, "Place the Invisible Man at the location with the most Items.\nDiscard all Items there.", 1, 3, {"InvisibleMan", "Dracula"}));

    monsterDeck.push_back(MonsterCard("TheDelivery", 3, "Place Wilbur & Chick at the Docks.", 1, 3, {"Frenzied Monster"}));

    monsterDeck.push_back(MonsterCard("FortuneTeller", 3, "Place Maleva at Camp.", 1, 2, {"Frenzied Monster"}));

    monsterDeck.push_back(MonsterCard("FormerEmployer", 3, "Place Dr. Cranley at Laboratory.", 1, 2, {"InvisibleMan", "Frenzied Monster"}));

    monsterDeck.push_back(MonsterCard("HurriedAssistant", 3, "Place Fritz at the Tower.", 2, 3, {"Dracula"}));

    monsterDeck.push_back(MonsterCard("TheInnocent", 3, "Place Maria at the Barn", 1, 3, {"Frenzied Monster", "Dracula", "InvisibleMan"}));

    monsterDeck.push_back(MonsterCard("EgyptionExpert", 3, "Place Prof. Pearson at the Cave", 2, 2, {"Dracula", "Frenzied Monster"}));

    monsterDeck.push_back(MonsterCard("TheIchthyologist", 3, "Place Dr. Reed at the Institute", 1, 2, {"Frenzied Monster"}));

    monsterDeck.push_back(MonsterCard("HypnoticGaze", 2, "The nearest hero or villager to Dracula moves 1 space closer to him.", 1, 2, {"InvisibleMan"}));
    monsterDeck.push_back(MonsterCard("HypnoticGaze", 2, "The nearest hero or villager to Dracula moves 1 space closer to him.", 1, 2, {"InvisibleMan"}));

    monsterDeck.push_back(MonsterCard("OnTheMove", 3, "Give the frenzy marker to the next Monster,\nand move each villager one space toward their safe location.", 3, 2, {"Frenzied Monster"}));
    monsterDeck.push_back(MonsterCard("OnTheMove", 3, "Give the frenzy marker to the next Monster,\nand move each villager one space toward their safe location.", 3, 2, {"Frenzied Monster"}));

    // بر زدن کارت‌ها
    random_device rd;
    mt19937 g(rd());
    shuffle(monsterDeck.begin(), monsterDeck.end(), g);
}

Hero *Game::getCurrentHero()
{
    if (currentHeroIndex >= 0 && currentHeroIndex < (int)heroes.size())
    {
        return heroes[currentHeroIndex];
    }
    throw GameException("Invalid hero index in getCurrentHero().");
}

void Game::nextTurn()
{
    currentHeroIndex = (currentHeroIndex + 1) % heroes.size();
}

bool Game::checkWinCondition() const
{
    int defeated = 0;

    for (Monster *m : monsters)
    {
        if (m->is_defeated())
        {
            defeated++;
        }
    }
    return defeated == 2; // اگر دو هیولا شکست بخورن، قهرمان‌ها برنده‌ان
}

bool Game::checkLoseCondition() const
{
    return terrorLevel >= maxTerror;
}

bool Game::checkOutOfTime() const
{ // اگه کارتای هیولا تموم شه زمان بازی به پایان رسیده
    return monsterDeck.empty() && !checkWinCondition();
}

void Game::rescueVillagerIfSafe(Hero *hero)
{
    //  بررسی تک‌تک محلی‌ها
    for (Villager *v : villagers)
    {
        // اگه محلی زنده باشه و به مکان امن خودش رسیده باشه
        if (v && v->isAlive() && v->isSafeLocation())
        {
            //  حذف از مکان فعلی
            if (v->getCurrentLocation())
            {
                v->getCurrentLocation()->removeVillager(v);
                v->setLocation(nullptr);
            }

            // علامت‌گذاری به‌عنوان نجات‌یافته
            v->markRescued(); // alive = false + rescued = true
            rescuedVillager = v;

            //  تلاش برای دادن کارت پرک
            if (hero)
            {
                try
                {
                    PerkCard reward = drawPerkCard();
                    hero->setPerkCard(reward);
                    rewardedPerkCard = reward;
                    setGameState(GameState::ShowPerkCard);
                    // cout << v->getName() << " was rescued and gave you a Perk Card!\n";
                }
                catch (const GameException &e)
                {
                    cout << v->getName() << " was rescued, but no perk card was left.\n";
                }
            }
        }
    }
}

PerkCard Game::drawPerkCard()
{
    if (perkDeck.empty())
    {
        throw GameException("No more perk cards left!");
    }
    PerkCard card = perkDeck.back();
    perkDeck.pop_back();
    return card;
}

void Game::usePerkCard(int index, std::vector<Location *> locs)
{
    Hero *hero = getCurrentHero();

    if (index == -1)
    { // بازیکن کارت نداره
        return;
    }

    PerkCard card;
    try
    {
        card = hero->usePerkCard(index);
    }
    catch (const exception &e)
    {
        cout << e.what();
        return;
    }

    try
    {
        switch (card.getType())
        {

        case PerkType::VISIT_FROM_THE_DETECTIVE:
        {
            Location *loc = locs[0];
            if (loc)
            {
                for (Monster *mons : monsters)
                {
                    if (auto *man = dynamic_cast<Invisible_Man *>(mons))
                    {

                        Location *oldLoc = man->get_location();
                        if (oldLoc && oldLoc != loc)
                        {
                            oldLoc->removeMonster(man);
                        }
                        loc->addMonster(man);
                        break;
                    }
                }
                break;
            }
            break;
        }
        case PerkType::BREAK_OF_DAWN:
        {
            skipNextMonsterPhase = true;
            for (int i = 0; i < 2; i++)
            {
                Item *item = ItemManager::getInstance().getRandomItem();
                if (!item)
                {
                    continue;
                }
                Location *loc = item->get_location();
                if (loc)
                {
                    loc->addItem(item);
                }
            }
            break;
        }
        case PerkType::OVERSTOCK:
        {
            Item *i1 = ItemManager::getInstance().getRandomItem();
            if (i1)
            {
                i1->get_location()->addItem(i1);
            }

            Item *i2 = ItemManager::getInstance().getRandomItem();
            if (i2)
            {
                i2->get_location()->addItem(i2);
            }
            break;
        }
        case PerkType::LATE_INTO_THE_NIGHT:
        {
            hero->addAction(2);
            break;
        }
        case PerkType::REPEL:
        {
            if (locs.empty())
            {
                cerr << "No locations selected.\n";
                break;
            }

            int locIndex = 0;
            for (auto monster : monsters)
            {
                if (!monster || monster->is_defeated())
                    continue; // از هیولاهای مرده عبور کن

                if (locIndex >= locs.size())
                {
                    cerr << "Not enough locations selected for all alive monsters.\n";
                    break;
                }

                Location *targetLoc = locs[locIndex++];
                Location *oldLoc = monster->get_location();

                if (oldLoc && oldLoc != targetLoc)
                {
                    oldLoc->removeMonster(monster);
                }
                targetLoc->addMonster(monster);
            }

            break;
        }
        case PerkType::HURRY:
        {
            if (locs.size() < 2)
            {
                cerr << "Not enough locations selected.\n";
                break;
            }

            auto heroes = Game::getInstance().getHeroes();

            // هیروی اول
            if (heroes.size() > 0 && heroes[0])
            {
                Location *targetLoc1 = locs[0];
                Location *oldLoc1 = heroes[0]->getLocation();

                if (oldLoc1 && oldLoc1 != targetLoc1)
                {
                    oldLoc1->removeHero(heroes[0]);
                }
                targetLoc1->addHero(heroes[0]);
            }

            // هیروی دوم
            if (heroes.size() > 1 && heroes[1])
            {
                Location *targetLoc2 = locs[1];
                Location *oldLoc2 = heroes[1]->getLocation();

                if (oldLoc2 && oldLoc2 != targetLoc2)
                {
                    oldLoc2->removeHero(heroes[1]);
                }
                targetLoc2->addHero(heroes[1]);
            }

            break;
        }
        }
    }
    catch (const exception &e)
    {
        cout << e.what();
    }
}

bool Game::canStartMonsterPhase()
{
    if (skipNextMonsterPhase)
    {
        skipNextMonsterPhase = false;
        currentState = GameState::EndMonsterPhase;
        return false;
    }
    return true;
}

void Game::drawMonsterCard()
{
    currentMosnterCard = monsterDeck.back();
    MonstersStrike = currentMosnterCard.get_strkTrgt();
    currentStrikeIndex = 0;
    monsterDeck.pop_back();
}

vector<Item *> Game::placeItemsOnMap()
{
    ItemManager &itemManager = ItemManager::getInstance();
    vector<Item *> result;

    for (int i = 0; i < currentMosnterCard.get_useItem(); ++i)
    {
        Item *item = itemManager.getRandomItem();
        if (item && item->get_location())
        {
            item->get_location()->addItem(item);
            result.push_back(item);
        }
        else
        {
            cerr << "Faild to place items on map in the monster phase!\n";
        }
    }

    return result;
}

void Game::setupMonsterStrike()
{
    targetMonster = nullptr;
    if (MonstersStrike[currentStrikeIndex] == "Frenzied Monster")
    {
        targetMonster = frenzy;
    }
    else
    {
        for (Monster *mon : monsters)
        {
            if (mon->get_name() == MonstersStrike[currentStrikeIndex])
            {
                targetMonster = mon;
                break;
            }
        }
    }
    if (!targetMonster)
    {
        std::cerr << "WARNING : No matching monster found for strike index " << currentStrikeIndex << "!\n";
    }
}

bool Game::moveMonster()
{
    Location *prev = targetMonster->get_location();
    int movement = currentMosnterCard.get_numMove();
    targetMonster->moveTowardTarget(heroes, villagers, movement);
    Location *current = targetMonster->get_location();

    if (prev != current)
    {
        return true;
    }
    else
    {
        return false;
    }
}

vector<string> Game::rollingDice()
{
    vector<string> result;
    diceCount.clear();
    for (int i = 0; i < currentMosnterCard.get_diceCount(); ++i)
    {
        Face face = currentMosnterCard.rollDie();
        result.push_back(currentMosnterCard.faceToString(face));
        diceCount[face]++;
    }
    return result;
}

StrikeResult Game::diceStrikeFace()
{

    Location *current = targetMonster->get_location();
    Hero *targetHero = nullptr;
    Villager *targetVillager = nullptr;

    // فقط اولین هیرویی که در این مکان قرار دارد انتخاب می‌شود
    if (targetMonster->get_name() == "Dracula")
    {
        for (Hero *h : heroes)
        {
            if (h->getLocation() == current)
            {
                targetHero = h;
                break;
            }
        }
    }

    // اگر هیرو نبود، دنبال ویلیجر بگرد
    if (!targetHero)
    {
        for (Villager *v : villagers)
        {
            if (v->isAlive() && v->getCurrentLocation() == current)
            {
                targetVillager = v;
                break;
            }
        }
    }

    if (targetHero && targetMonster->get_name() == "Dracula")
    {
        damageHero = targetHero;
        return StrikeResult::HeroFound;
    }
    else if (targetVillager)
    {
        damageVillager = targetVillager;
        targetVillager->kill();
        terrorLevel++;
        targetVillager->getCurrentLocation()->removeVillager(targetVillager);
        targetVillager->setLocation(nullptr);
        return StrikeResult::VillagerFound;
    }
    else
    {
        return StrikeResult::HeroAndVillagerNotFound;
    }
}

void Game::sendHeroToHospital()
{
    if (!damageHero)
    {
        return;
    }

    terrorLevel++;
    damageHero->getLocation()->removeHero(damageHero); // حذف قهرمان از لوکیشن قبلی
    map->getLocation("Hospital")->addHero(damageHero); // اضافه کردن قهرمان به لوکیشن فعلی
}

void Game::defendHero(Item *item)
{
    damageHero->removeItem(item);
    ItemManager::getInstance().recycleItemToUsedItems(item);
}

void Game::useMonsterCard(const string &NAME)
{
    map = Map::get_instanse();
    event = {};

    try
    {
        if (NAME == "FormOfTheBat")
        {
            Monster *dracula = nullptr;

            for (Monster *m : monsters)
            { // پیدا کردن دراکولا در لیست هیولاها
                if (m->get_name() == "Dracula")
                {
                    dracula = m;
                    break;
                }
            }

            // بررسی وجود Dracula و قهرمان فعلی
            Hero *currentHero = getCurrentHero();
            if (dracula && currentHero)
            {
                Location *heroLoc = currentHero->getLocation(); // مکان قهرمان فعلی
                Location *oldLoc = dracula->get_location();     // مکان قبلی دراکولا

                if (oldLoc && oldLoc != heroLoc)
                {                                   // اگر قبلاً اونجا نبوده
                    oldLoc->removeMonster(dracula); // حذف از مکان قدیمی
                }
                heroLoc->addMonster(dracula);

                event.monsterName = "Dracula";
                event.heroName = currentHero->getClassName();
                event.locationName = currentHero->getLocation()->get_name();

                event.msg = "Dracula has flown to " + heroLoc->get_name();
            }
            else
            {
                cerr << "Could not move Dracula. Hero or Dracula missing.\n";
            }
        }
        else if (NAME == "Thief")
        {
            ItemManager &itemManager = ItemManager::getInstance();

            //  پیدا کردن Invisible Man
            Monster *invisible = nullptr;
            for (Monster *m : monsters)
            {
                if (m->get_name() == "InvisibleMan")
                {
                    invisible = m;
                    break;
                }
            }

            if (!invisible)
            {
                cerr << "Invisible Man not found!\n";
                return;
            }

            // پیدا کردن مکانی با بیشترین آیتم
            Location *targetLoc = nullptr;
            int maxCount = -1;

            for (Location *loc : map->getAllLocations())
            {
                int itemCount = loc->get_items().size();
                if (itemCount > maxCount)
                {
                    maxCount = itemCount;
                    targetLoc = loc;
                }
            }

            if (!targetLoc || maxCount == 0)
            {
                cerr << "No items found on map to steal!\n";
                return;
            }

            // انتقال Invisible Man به آن مکان
            Location *oldLoc = invisible->get_location(); // مکان قبلی invisible Man

            if (oldLoc && oldLoc != targetLoc)
            {
                oldLoc->removeMonster(invisible);
            }

            targetLoc->addMonster(invisible);

            // حذف آیتم‌ها و انتقال به usedItems

            while (!targetLoc->get_items().empty())
            {
                Item *item = targetLoc->get_items().back();
                targetLoc->removeItem(item);
                itemManager.recycleItemToUsedItems(item);
            }

            event.monsterName = "InvisibleMan";
            event.locationName = targetLoc->get_name();

            event.msg = "Invisible Man moved to " + targetLoc->get_name() + "\nand stole all " + to_string(maxCount) + " items!";
        }
        else if (NAME == "Sunrise")
        {
            //  پیدا کردن Dracula
            Monster *dracula = nullptr;
            for (Monster *m : monsters)
            {
                if (m->get_name() == "Dracula")
                {
                    dracula = m;
                    break;
                }
            }

            if (!dracula)
            {
                cerr << "Dracula not found!\n";
                return;
            }

            // پیدا کردن مکان Crypt
            Location *crypt = map->getLocation("Crypt");
            if (!crypt)
            {
                cerr << "Crypt location not found!\n";
                return;
            }

            //  انتقال Dracula به Crypt
            Location *currentLoc = dracula->get_location();
            if (currentLoc && currentLoc != crypt)
            {
                currentLoc->removeMonster(dracula);
            }

            crypt->addMonster(dracula);

            event.monsterName = "Dracula";
            event.locationName = "Crypt";

            event.msg = "Dracula retreated to the Crypt as the sun rises.";
        }
        else if (NAME == "TheDelivery")
        {
            Location *docks = map->getLocation("Docks");
            if (!docks)
            {
                cerr << "Location 'Docks' not found!\n";
                return;
            }

            villagers.push_back(new Villager("WilburAndChick", nullptr, map->getLocation("Dungeon")));

            // اضافه کردن به مکان Docks
            docks->addVillager(villagers.back());

            event.villagerName = "WilburAndChick";
            event.locationName = "Docks";

            event.msg = "Wilbur & Chick have arrived at the Docks!";
        }
        else if (NAME == "FortuneTeller")
        {
            Location *camp = map->getLocation("Camp");
            if (!camp)
            {
                cerr << "Location 'Camp' not found!\n";
                return;
            }

            villagers.push_back(new Villager("Maleva", nullptr, map->getLocation("Shop")));

            // اضافه کردن به مکان camp
            camp->addVillager(villagers.back());

            event.villagerName = "Maleva";
            event.locationName = "Camp";

            event.msg = "Maleva have arrived at the Camp!";
        }
        else if (NAME == "FormerEmployer")
        {
            Location *lab = map->getLocation("Laboratory");
            if (!lab)
            {
                cerr << "Location 'Laboratory' not found!\n";
                return;
            }

            villagers.push_back(new Villager("DrCranly", nullptr, map->getLocation("Precinct")));

            lab->addVillager(villagers.back());

            event.villagerName = "DrCranly";
            event.locationName = "Laboratory";

            event.msg = "Dr. Cranly have arrived at the Laboratory!";
        }
        else if (NAME == "HurriedAssistant")
        {
            Location *tower = map->getLocation("Tower");
            if (!tower)
            {
                cerr << "Location 'Tower' not found!\n";
                return;
            }

            villagers.push_back(new Villager("Fritz", nullptr, map->getLocation("Institute")));

            tower->addVillager(villagers.back());

            event.villagerName = "Fritz";
            event.locationName = "Tower";
            event.msg = "Fritz have arrived at the Tower!";
        }
        else if (NAME == "TheInnocent")
        {
            Location *barn = map->getLocation("Barn");
            if (!barn)
            {
                cerr << "Location 'Barn' not found!\n";
                return;
            }

            villagers.push_back(new Villager("Maria", nullptr, map->getLocation("Camp")));

            barn->addVillager(villagers.back());

            event.villagerName = "Maria";
            event.locationName = "Barn";

            event.msg = "Maria have arrived at the Barn!";
        }
        else if (NAME == "EgyptionExpert")
        {
            Location *cave = map->getLocation("Cave");
            if (!cave)
            {
                cerr << "Location 'Cave' not found!\n";
                return;
            }

            villagers.push_back(new Villager("ProfPearson", nullptr, map->getLocation("Museum")));

            cave->addVillager(villagers.back());

            event.villagerName = "ProfPearson";
            event.locationName = "Cave";

            event.msg = "Prof. Pearson have arrived at the Cave!";
        }
        else if (NAME == "TheIchthyologist")
        {
            Location *ins = map->getLocation("Institute");
            if (!ins)
            {
                cerr << "Location 'Institute' not found!\n";
                return;
            }

            villagers.push_back(new Villager("DrReed", nullptr, map->getLocation("Camp")));

            ins->addVillager(villagers.back());

            event.villagerName = "DrReed";
            event.locationName = "Institute";

            event.msg = "Dr. Reed have arrived at the Institute!";
        }
        else if (NAME == "HypnoticGaze")
        {
            Monster *dracula = nullptr;
            for (Monster *m : monsters)
            {
                if (m->get_name() == "Dracula")
                {
                    dracula = m;
                    break;
                }
            }

            if (!dracula)
            {
                cerr << "Dracula not found!\n";
                return;
            }

            Location *draculaLoc = dracula->get_location();
            if (!draculaLoc)
            {
                cerr << "Dracula has no location!\n";
                return;
            }

            // پیدا کردن نزدیکترین هیرو یا ویلیجر
            Hero *closesHero = nullptr;
            Villager *closesVillager = nullptr;
            size_t minDistance = INT_MAX;

            for (Hero *h : heroes)
            { // بررسی فاصله هیرو و دراکولا
                Location *heroLoc = h->getLocation();
                if (!heroLoc)
                {
                    continue;
                }
                int distance = dracula->findShortestPath(heroLoc, draculaLoc).size();
                if (distance <= 1)
                {
                    continue;
                }
                if (distance < minDistance)
                {
                    minDistance = distance;
                    closesHero = h;
                    closesVillager = nullptr;
                }
            }

            for (Villager *v : villagers)
            { // بررسی فاصله محلی و دراکولا
                if (!v->isAlive())
                {
                    continue;
                }
                Location *villLoc = v->getCurrentLocation();
                if (!villLoc)
                {
                    continue;
                }
                int distance = dracula->findShortestPath(villLoc, draculaLoc).size();
                if (distance <= 1)
                {
                    continue;
                }
                if (distance < minDistance)
                {
                    minDistance = distance;
                    closesHero = nullptr;
                    closesVillager = v;
                }
            }

            // حرکت به سمت دراکولا
            if (closesHero)
            {
                Location *current = closesHero->getLocation();
                vector<Location *> path = dracula->findShortestPath(current, draculaLoc);

                std::cout << "Checking path for hero: " << closesHero->getClassName()
                          << " | Path size: " << path.size() << '\n';

                if (path.size() >= 2)
                {
                    Location *next = path[1];
                    current->removeHero(closesHero);
                    next->addHero(closesHero);

                    event.heroName = closesHero->getClassName();
                    event.locationName = next->get_name();
                    event.msg = closesHero->getClassName() + " was drawn 1 space closer to dracula.\nand go to " + next->get_name();
                }
            }
            else if (closesVillager)
            {
                Location *current = closesVillager->getCurrentLocation();
                vector<Location *> path = dracula->findShortestPath(current, draculaLoc);
                if (path.size() >= 2)
                {
                    Location *next = path[1];
                    current->removeVillager(closesVillager);
                    next->addVillager(closesVillager);

                    event.villagerName = closesVillager->getName();
                    event.locationName = next->get_name();
                    event.msg = closesVillager->getName() + " was drawn 1 space closer to dracula.";
                }
            }
        }
        else if (NAME == "OnTheMove")
        {
            updateFrenzy();
            event.monsterName = frenzy->get_name();
            // cout << "Frenzy marker moved to: " << frenzy->get_name() << '\n';

            for (Villager *v : villagers)
            {
                if (!v->isAlive() || v->isSafeLocation())
                {
                    continue;
                }

                Location *currentLoc = v->getCurrentLocation();
                Location *safeLoc = v->getSafeLocation();
                if (!currentLoc || !safeLoc)
                {
                    continue;
                }

                vector<Location *> path = frenzy->findShortestPath(currentLoc, safeLoc);
                if (path.size() >= 2)
                {
                    Location *next = path[1];
                    currentLoc->removeVillager(v);
                    next->addVillager(v);
                    event.villagersAndLocations.push_back({v->getName(), next->get_name()});
                }
            }
        }
    }
    catch (const exception &e)
    {
        cout << e.what();
    }
    rescueVillagerIfSafe(nullptr);
}

Game::~Game()
{
    for (Monster *&m : monsters)
    {
        if (m != nullptr)
        {
            delete m;
            m = nullptr;
        }
    }
    monsters.clear();
    // حذف قهرمان‌ها
    for (Hero *&h : heroes)
    {
        if (h != nullptr)
        {
            delete h;
            h = nullptr;
        }
    }
    heroes.clear();
    // حذف Villagerها
    for (Villager *&v : villagers)
    {
        if (v != nullptr)
        {
            delete v;
            v = nullptr;
        }
    }
    villagers.clear();

    monsterDeck.clear();
    perkDeck.clear();
}

void Game::reset()
{
    for (Monster *m : monsters)
    {
        if (m != nullptr)
        {
            delete m;
            m = nullptr;
        }
    }
    monsters.clear();
    // حذف قهرمان‌ها
    for (Hero *&h : heroes)
    {
        if (h != nullptr)
        {
            delete h;
            h = nullptr;
        }
    }
    heroes.clear();
    // حذف Villagerها
    for (Villager *&v : villagers)
    {
        if (v != nullptr)
        {
            delete v;
            v = nullptr;
        }
    }
    villagers.clear();

    for (Location *loc : map->getAllLocations())
    {
        if (loc)
        {
            loc->clearAll();
        }
    }

    monsterDeck.clear();
    perkDeck.clear();

    ItemManager::getInstance().clear();

    currentHeroIndex = 0;
    frenzy = nullptr;
    terrorLevel = 0;
    maxTerror = 5;
    skipNextMonsterPhase = false;

    player1 = {};
    player2 = {};
    player1_time = 0;
    player2_time = 0;

    map = Map::get_instanse();

    setLoadedFromFile(false); // ← چون بازی از فایل نیست، پس false
}

Monster *Game::getFrenzy() const
{
    return frenzy;
}

void Game::updateFrenzy()
{
    if (monsters.empty())
    {
        cout << "No monster exit!\n";
        return;
    }
    vector<Monster *> aliveMonsters;

    for (Monster *m : monsters)
    {
        if (!m->is_defeated())
        {
            aliveMonsters.push_back(m);
        }
    }

    if (aliveMonsters.empty())
    {
        frenzy = nullptr;
        return;
    }

    sort(aliveMonsters.begin(), aliveMonsters.end(), [](Monster *a, Monster *b)
         { return a->getFrenzyOrder() < b->getFrenzyOrder(); });

    auto it = find(aliveMonsters.begin(), aliveMonsters.end(), frenzy);

    if (it == aliveMonsters.end() || ++it == aliveMonsters.end())
    {
        frenzy = aliveMonsters.front();
    }
    else
    {
        frenzy = *it;
    }
}

int Game::getTerrorLevel() const
{
    return terrorLevel;
}

void Game::setTerrorLevel(int level)
{
    terrorLevel = level;
}

int Game::getCurrentHeroIndex() const
{
    return currentHeroIndex;
}

void Game::setCurrentHeroIndex(int index)
{
    currentHeroIndex = index;
}

int Game::getPlayer1Time() const
{
    return player1_time;
}

int Game::getPlayer2Time() const
{
    return player2_time;
}

void Game::setPlayerTimes(int time1, int time2)
{
    player1_time = time1;
    player2_time = time2;
}

bool Game::getSkipNextMonsterPhase() const
{
    return skipNextMonsterPhase;
}

void Game::setSkipNextMonsterPhase(bool skip)
{
    skipNextMonsterPhase = skip;
}

Monster *Game::getFrenzyMonster() const
{
    return frenzy;
}

void Game::setFrenzyMonster(Monster *m)
{
    frenzy = m;
}

vector<Monster *> Game::getMonsters() const { return monsters; }

void Game::setMonsters(const std::vector<Monster *> &newMonsters)
{
    // حذف هیولاهای قبلی از مکان‌شون و آزاد کردن حافظه
    for (Monster *m : monsters)
    {
        if (m && m->get_location())
        {
            m->get_location()->removeMonster(m);
        }
        delete m;
    }

    monsters.clear();
    monsters = newMonsters;
    frenzy = nullptr;
}

vector<Villager *> Game::getVillagers() const { return villagers; }
void Game::setVillagers(const vector<Villager *> &v)
{
    villagers = v;
}

vector<PerkCard> Game::getPerkDeck() const { return perkDeck; }
void Game::setPerkDeck(const vector<PerkCard> &p) { perkDeck = p; }

vector<MonsterCard> Game::getMonsterDeck() const { return monsterDeck; }
void Game::setMonsterDeck(const vector<MonsterCard> &m)
{
    monsterDeck = m;
}

std::vector<Hero *> Game::getHeroes() const { return heroes; }

void Game::setHeroes(const std::vector<Hero *> &h)
{
    if (h.size() != 2)
        throw GameException("Expected exactly 2 heroes to load!");

    this->heroes = h;

    // استخراج نام و کلاس هیروها برای پلیر ۱ و ۲
    player1.name = h[0]->getName();
    player1.heroClass = h[0]->getClassName();

    player2.name = h[1]->getName();
    player2.heroClass = h[1]->getClassName();
}

void Game::setSlot(int slot)
{
    if (slot >= 1)
        Slot = slot;
    else
        throw GameException("Slot must be between 1 and 5");
}

int Game::findNextFreeSlot() const
{
    int i = 1;
    while (true)
    {
        std::ifstream file("save" + std::to_string(i) + ".txt");
        if (!file.good())
        {
            return i; // اولین فایل خالی پیدا شد
        }
        ++i;
    }
}

void Game::setCurrentSaveSlot(int slot) { currentSaveSlot = slot; }
int Game::getCurrentSaveSlot() const { return currentSaveSlot; }

void Game::setLoadedFromFile(bool b) { isLoadedFromFile = b; }
bool Game::getLoadedFromFile() const { return isLoadedFromFile; }

void Game::handleGameOver()
{
    std::cout << "\n--- GAME OVER ---\n";

    if (getLoadedFromFile() && currentSaveSlot > 0)
    {
        std::string filename = "save" + std::to_string(currentSaveSlot) + ".txt";
        if (std::remove(filename.c_str()) == 0)
        {
            std::cout << "Save file deleted: " << filename << "\n";
        }
        else
        {
            std::cerr << "Error deleting save file: " << filename << "\n";
        }
    }

    exit(0);
}

void Game::handleMove(Location *loc, std::vector<Villager *> vills)
{
    getCurrentHero()->move(loc, vills);
}

void Game::handlePickUP(std::vector<Item *> items)
{
    getCurrentHero()->pickUp(items);
}

void Game::handleGuide(std::vector<Villager *> v, Location *dest)
{
    getCurrentHero()->guide(v, dest);
}

void Game::handleAdvance(Monster *mon, std::vector<Item *> items)
{
    getCurrentHero()->advanced(mon, items);
}

void Game::handleDefeat(Monster *mon, std::vector<Item *> items)
{
    getCurrentHero()->defeat(mon, items);
    //  حذف از مکان فعلی
    if (getCurrentHero()->getLocation())
    {
        getCurrentHero()->getLocation()->removeMonster(mon);
        mon->set_location(nullptr);
    }
    //}
}

void Game::handleSpecialAction(Location *selectedLocation, const std::vector<Item *> &selectedItems)
{

    Hero *hero = getCurrentHero();

    if (auto *arch = dynamic_cast<Archaeologist *>(hero))
    {
        if (!selectedLocation)
            throw GameException("No location selected for Archaeologist special action!");
        arch->specialAction(selectedItems);
    }
    else if (auto *courier = dynamic_cast<Courier *>(hero))
    {
        courier->specialAction({});
    }
    else
    {
        throw GameException("This hero doesn't have a special action.");
    }
}

vector<Hero *> Game::getAllHeroes() const
{
    return heroes;
}

void Game::prepareForSaving()
{
    if (getCurrentSaveSlot() != 0)
        return; // اگه قبلاً اسلات اختصاص داده شده، دیگه کاری نکن

    int newSlot = findNextFreeSlot();
    setSlot(newSlot);
    setCurrentSaveSlot(newSlot);
    setLoadedFromFile(false); // چون بازی جدید بوده، لود نشده
}

bool Game::shouldShowSpecialAcion()
{
    Hero *hero = getCurrentHero();
    if (hero->getClassName() == "Courier" || hero->getClassName() == "Archaeologist")
    {
        return true;
    }
    return false;
}