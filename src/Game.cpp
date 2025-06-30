#include "Game.hpp"
#include <algorithm>
#include <random>
#include <cstdlib>
#include <stdexcept>

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

void Game::boundary() const
{
    cout << "--------------------------------------------------------------------------------------------\n";
}

void Game::showGameStatuse()
{
    const int locWidth = 15;
    const int itemWidth = 30;
    const int monWidth = 20;
    const int villWidth = 25;
    const int heroWidth = 15;

    boundary();
    cout << left
         << setw(locWidth) << "Location"
         << setw(itemWidth) << "Items"
         << setw(monWidth) << "Monsters"
         << setw(villWidth) << "Villagers"
         << setw(heroWidth) << "Heroes"
         << "\n";
    boundary();

    vector<Location *> locs = map->getAllLocations();
    for (const auto &loc : locs)
    {
        string locName = loc->get_name();
        vector<string> itemLines = getItemLines(loc->get_items());
        vector<string> monsterLines = getMonsterLines(loc->get_monsters());
        vector<string> villagerLines = getVillagerLines(loc->get_villagers());
        vector<string> heroLines = getHeroLines(loc->get_heroes());

        // پیدا کردن حداکثر تعداد خطوط برای یک مکان
        size_t maxLines = max({itemLines.size(), monsterLines.size(), villagerLines.size(), heroLines.size()});
        maxLines = max(maxLines, size_t(1)); // حداقل یک خط

        for (size_t i = 0; i < maxLines; ++i)
        { // فقط خط اول، نام مکان رو چاپ میکنه
            if (i == 0)
            {
                cout << left << setw(locWidth) << locName;
            }
            else
            {
                cout << left << setw(locWidth) << "";
            }

            // چاپ آیتم‌ها
            cout << left << setw(itemWidth) << (i < itemLines.size() ? itemLines[i] : "");

            // چاپ هیولاها
            cout << left << setw(monWidth) << (i < monsterLines.size() ? monsterLines[i] : "");

            // چاپ ویلیجرها
            cout << left << setw(villWidth) << (i < villagerLines.size() ? villagerLines[i] : "");

            // چاپ هیروها
            cout << left << setw(heroWidth) << (i < heroLines.size() ? heroLines[i] : "");

            cout << "\n";
        }
    }
    boundary();
    cout << "Key Locations:\n";
    if (monsters[0]->get_name() == "Dracula")
    {
        cout << "[Castle] Coffins smashed: (" << monsters[0]->getCounter() << "/4)\n";
        cout << "[Lab] Evidence collected: (" << monsters[1]->getCounter() << "/5)\n";
    }
    else
    {
        cout << "[Castle] Coffins smashed: (" << monsters[1]->getCounter() << "/4)\n";
        cout << "[Lab] Evidence collected: (" << monsters[0]->getCounter() << "/5)\n";
    }
    cout << "**************************************************************\n";
    cout << "Terror Level: " << terrorLevel << endl;
    cout << "**************************************************************\n";
    boundary();
}

vector<string> Game::getItemLines(const vector<Item *> &items)
{
    vector<string> lines;
    for (const auto &item : items)
    {
        if (item)
        {
            lines.push_back(item->get_name() + "(" + to_string(item->get_strength()) + ")" + "[" + item->get_color_to_string() + "]");
        }
    }
    return lines.empty() ? vector<string>{"-"} : lines;
}

vector<string> Game::getMonsterLines(const vector<Monster *> &monsters)
{
    vector<string> lines;
    for (const auto &monster : monsters)
    {
        if (monster)
        {
            lines.push_back(monster->get_name() + (frenzy == monster ? "(F)" : ""));
        }
    }
    return lines.empty() ? vector<string>{"-"} : lines;
}

vector<string> Game::getVillagerLines(const vector<Villager *> &villagers)
{
    vector<string> lines;
    for (const auto &villager : villagers)
    {
        if (villager && villager->getSafeLocation())
        {
            lines.push_back(villager->getName() + "(" + villager->getSafeLocation()->get_name() + ")");
        }
    }
    return lines.empty() ? vector<string>{"-"} : lines;
}

vector<string> Game::getHeroLines(const vector<Hero *> &heroes)
{
    vector<string> lines;
    for (const auto &hero : heroes)
    {
        if (hero && !hero->getName().empty())
        {
            lines.push_back(hero->getName());
        }
    }

    return lines.empty() ? vector<string>{"-"} : lines;
}

void Game::info()
{
    boundary();
    Hero *hero = getCurrentHero();
    cout << "Hero Info\n";
    cout << "\t" << "Hero: " << hero->getClassName() << "(" << hero->getName() << ")" << endl;
    cout << "\t" << "Action left: " << hero->getActionsLeft() << "/" << hero->getMaxActions() << endl;
    cout << "\t" << "Items: " << hero->getItemSummary();
    cout << "\tPerks: " << hero->getPerkSummary();
}

void Game::start()
{
    setup(); // آماده سازی اولیه بازی
    while (true)
    {
        showGameStatuse();
        Hero *current = getCurrentHero();

        if (!current)
        {
            cout << "Error : No current Hero!\n";
            break;
        }

        heroPhase(current); // اجرای فاز قهرمان

        showGameStatuse();

        if (checkWinCondition())
        { // بررسی شرط برد
            cout << "You defeated all monsters! You won!\n";
            break;
        }
        if (checkLoseCondition())
        { // بررسی شرط باخت
            cout << "The city fell into terror. Game over.\n";
            break;
        }

        monsterPhase(); // اجرای فاز هیولا

        if (checkWinCondition())
        { // بررسی دوباره شرط برد
            cout << "You defeated all monsters! You won!\n";
            break;
        }
        if (checkLoseCondition())
        { // بررسی دوباره شرط باخت
            cout << "The city fell into terror. Game over.\n";
            break;
        }
        if (checkOutOfTime())
        { // بررسی پایان زمان بازی
            cout << "The monsters endured... Time's up. Game over.\n";
            break;
        }
        nextTurn();
    }
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

    Hero *hero1 = nullptr; // ساخت قهرمان اول
    Hero *hero2 = nullptr; // ساخت قهرمان دوم

    if (player1.heroClass == "Mayor")
    {
        hero1 = new Mayor(player1.name, map);
    }
    else
    {
        hero1 = new Archaeologist(player1.name, map);
    }

    if (player2.heroClass == "Mayor")
    {
        hero2 = new Mayor(player2.name, map);
    }
    else
    {
        hero2 = new Archaeologist(player2.name, map);
    }

    if (currentHeroIndex == 0)
    { // ترتیب نوبت دهی قهرمان ها
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

    monsterDeck.push_back(MonsterCard("Form of The Bat", 2, "Place Dracula in the space with the current Player's Hero.", 1, 2, {"Invisible Man"}));
    monsterDeck.push_back(MonsterCard("Form of The Bat", 2, "Place Dracula in the space with the current Player's Hero.", 1, 2, {"Invisible Man"}));
    monsterDeck.push_back(MonsterCard("Form of The Bat", 2, "Place Dracula in the space with the current Player's Hero.", 1, 2, {"Invisible Man"}));

    monsterDeck.push_back(MonsterCard("Sunrise", 0, "Place Dracula at the crypt.", 1, 2, {"Invisible Man", "Frenzied Monster"}));
    monsterDeck.push_back(MonsterCard("Sunrise", 0, "Place Dracula at the crypt.", 1, 2, {"Invisible Man", "Frenzied Monster"}));
    monsterDeck.push_back(MonsterCard("Sunrise", 0, "Place Dracula at the crypt.", 1, 2, {"Invisible Man", "Frenzied Monster"}));

    monsterDeck.push_back(MonsterCard("Thief", 2, "Place the Invisible Man at the location with the most Items.\nDiscard all Items there.", 1, 3, {"Invisible Man", "Dracula"}));
    monsterDeck.push_back(MonsterCard("Thief", 2, "Place the Invisible Man at the location with the most Items.\nDiscard all Items there.", 1, 3, {"Invisible Man", "Dracula"}));
    monsterDeck.push_back(MonsterCard("Thief", 2, "Place the Invisible Man at the location with the most Items.\nDiscard all Items there.", 1, 3, {"Invisible Man", "Dracula"}));
    monsterDeck.push_back(MonsterCard("Thief", 2, "Place the Invisible Man at the location with the most Items.\nDiscard all Items there.", 1, 3, {"Invisible Man", "Dracula"}));
    monsterDeck.push_back(MonsterCard("Thief", 2, "Place the Invisible Man at the location with the most Items.\nDiscard all Items there.", 1, 3, {"Invisible Man", "Dracula"}));

    monsterDeck.push_back(MonsterCard("The Delivery", 3, "Place Wilbur & Chick at the Docks.", 1, 3, {"Frenzied Monster"}));

    monsterDeck.push_back(MonsterCard("Fortune Teller", 3, "Place Maleva at Camp.", 1, 2, {"Frenzied Monster"}));

    monsterDeck.push_back(MonsterCard("Former Employer", 3, "Place Dr. Cranley at Laboratory.", 1, 2, {"Invisible Man", "Frenzied Monster"}));

    monsterDeck.push_back(MonsterCard("Hurried Assistant", 3, "Place Fritz at the Tower.", 2, 3, {"Dracula"}));

    monsterDeck.push_back(MonsterCard("The Innocent", 3, "Place Maria at the Barn", 1, 3, {"Frenzied Monster", "Dracula", "Invisible Man"}));

    monsterDeck.push_back(MonsterCard("Egyption Expert", 3, "Place Prof. Pearson at the Cave", 2, 2, {"Dracula", "Frenzied Monster"}));

    monsterDeck.push_back(MonsterCard("The Ichthyologist", 3, "Place Dr. Reed at the Institute", 1, 2, {"Frenzied Monster"}));

    monsterDeck.push_back(MonsterCard("Hypnotic Gaze", 2, "The nearest hero or villager to Dracula moves 1 space closer to him.", 1, 2, {"Invisible Man"}));
    monsterDeck.push_back(MonsterCard("Hypnotic Gaze", 2, "The nearest hero or villager to Dracula moves 1 space closer to him.", 1, 2, {"Invisible Man"}));

    monsterDeck.push_back(MonsterCard("On The Move", 3, "Give the frenzy marker to the next Monster,\nand move each villager one space toward their safe location.", 3, 2, {"Frenzied Monster"}));
    monsterDeck.push_back(MonsterCard("On The Move", 3, "Give the frenzy marker to the next Monster,\nand move each villager one space toward their safe location.", 3, 2, {"Frenzied Monster"}));

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

            //  تلاش برای دادن کارت پرک
            if (hero)
            {
                try
                {
                    PerkCard reward = drawPerkCard();
                    hero->setPerkCard(reward);
                    cout << v->getName() << " was rescued and gave you a Perk Card!\n";
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

string Game::actionMenu()
{
    boundary();
    showGameStatuse();
    info();
    boundary();
    cout << "[M]ove                       " << "|\t" << "[G]uide\n";
    cout << "[P]ick Up                    " << "|\t" << "[A]dvance\n";
    cout << "[D]efeat                     " << "|\t" << "[U]se Perk\n";
    cout << "[H]elp                       " << "|\t" << "[Q]uit\n";
    cout << "[S]pecial(for Archaeologist)\n";
    boundary();

    cout << "Enter your choice[As string]:\t";
    string choice;
    getline(cin, choice);
    return choice;
}

Location *Game::askLocationChoice(const vector<Location *> &loc)
{
    boundary();

    if (loc.empty())
    {
        cout << "No available locations to choose from.\n";
        boundary();
        return nullptr;
    }

    while (true)
    {
        cout << "Available Locations:\n";
        for (size_t i = 0; i < loc.size(); ++i)
        {
            cout << "  " << i + 1 << ". " << loc[i]->get_name() << '\n';
        }

        cout << "\nChoose one of the locations by number: ";
        string input;
        getline(cin, input);

        try
        {
            size_t pos;
            int index = stoi(input, &pos);

            // بررسی معتبر بودن ورودی
            if (pos != input.size() || index < 1 || index > (int)loc.size())
            {
                throw invalid_argument("Out of range");
            }

            boundary();
            return loc[index - 1];
        }
        catch (...)
        {
            cout << "Invalid input! Please enter a number between 1 and " << loc.size() << ".\n";
        }
    }
}

vector<Villager *> Game::askVillagerChoice(const vector<Villager *> &villag)
{
    boundary();
    vector<Villager *> selected;

    if (villag.empty())
    {
        cout << "No villagers available to choose from.\n";
        boundary();
        return selected;
    }

    vector<Villager *> remaining = villag;

    while (!remaining.empty())
    {
        cout << "Available Villagers:\n";
        cout << "  0. Don't choose any villager\n";
        for (size_t i = 0; i < remaining.size(); ++i)
        {
            cout << "  " << i + 1 << ". " << remaining[i]->getName() << '\n';
        }

        cout << "\nChoose one of the villagers by number: ";
        string input;
        getline(cin, input);

        int index = -1;
        try
        {
            size_t pos;
            index = stoi(input, &pos);
            if (pos != input.size() || index < 0 || index > (int)remaining.size())
            {
                throw invalid_argument("Out of range");
            }
        }
        catch (...)
        {
            cout << "Invalid input! Please enter a valid number.\n";
            continue;
        }

        if (index == 0)
        {
            return {};
        }

        Villager *picked = remaining[index - 1];
        selected.push_back(picked);
        remaining.erase(remaining.begin() + index - 1);

        cout << picked->getName() << " selected.\n";

        if (remaining.empty())
        {
            cout << "No more villagers left to choose from.\n";
            break;
        }

        cout << "Would you like to choose another one? (y to continue): ";
        char again;
        cin >> again;
        cin.ignore();

        if (tolower(again) != 'y')
            break;
    }

    boundary();
    return selected;
}

Item *Game::askItemToDefend(const std::vector<Item *> &ITEM)
{
    boundary();

    if (ITEM.empty())
    {
        cout << "No items available to choose from.\n";
        boundary();
        return nullptr;
    }

    while (true)
    {
        cout << "Available Items:\n";
        // گزینه‌ی اضافی برای عدم انتخاب
        cout << "  0. Don't defend (take damage)\n";
        for (size_t i = 0; i < ITEM.size(); ++i)
        {
            cout << "  " << i + 1 << ". " << ITEM[i]->get_name()
                 << " (" << ITEM[i]->get_strength() << ") ["
                 << ITEM[i]->get_color_to_string() << "]\n";
        }

        cout << "\nYour choice (0-" << ITEM.size() << "): ";
        string input;
        getline(cin, input);

        int index = -1;
        try
        {
            size_t pos;
            index = stoi(input, &pos);

            // بررسی تمام ورودی
            if (pos != input.size())
                throw invalid_argument("Extra characters");

            if (index == 0)
            {
                return nullptr; // انتخاب نکرده
            }
            else if (index >= 1 && index <= (int)ITEM.size())
            {
                return ITEM[index - 1]; // انتخاب معتبر
            }
            else
            {
                throw out_of_range("Out of range");
            }
        }
        catch (...)
        {
            cout << "Invalid input! Please enter a number between 0 and " << ITEM.size() << ".\n\n";
        }
    }
}

vector<Item *> Game::askItemSelection(const vector<Item *> &ITEM)
{
    boundary();
    vector<Item *> selectedItems;

    if (ITEM.empty())
    {
        cout << "No items available to choose from.\n";
        boundary();
        return selectedItems;
    }

    // نسخه کپی قابل ویرایش از لیست آیتم‌ها
    vector<Item *> remainingItems = ITEM;

    while (!remainingItems.empty())
    {
        cout << "Available Items:\n";
        for (size_t i = 0; i < remainingItems.size(); ++i)
        {
            cout << "  " << i + 1 << ". " << remainingItems[i]->get_name() << "(" << remainingItems[i]->get_strength() << ")" << "[" << remainingItems[i]->get_color_to_string() << "]" << '\n';
        }

        cout << "\nYour choice: ";
        string input;
        getline(cin, input);

        int index = -1;
        try
        {
            size_t pos;
            index = stoi(input, &pos);
            if (pos != input.size() || index < 1 || index > (int)remainingItems.size())
            {
                throw invalid_argument("Out of range");
            }
        }
        catch (...)
        {
            cout << "Invalid input! Please enter a valid number.\n";
            continue;
        }

        // اضافه کردن به لیست انتخاب شده و حذف از لیست قابل نمایش
        Item *pickedItem = remainingItems[index - 1];
        selectedItems.push_back(pickedItem);
        remainingItems.erase(remainingItems.begin() + index - 1);

        cout << pickedItem->get_name() << " selected.\n";

        if (remainingItems.empty())
        {
            cout << "No more items left in this location.\n";
            break;
        }

        cout << "Would you like to select another item? (y to continue,anything to break): ";
        string again;
        getline(cin, again);
        if (again != "y")
        {
            break;
        }
    }

    boundary();
    return selectedItems;
}

Monster *Game::askMonsterChoice(const vector<Monster *> &mon)
{
    boundary();

    if (mon.empty())
    {
        cout << "No monsters available to choose from.\n";
        boundary();
        return nullptr;
    }

    vector<Monster *> ans;
    for (Monster *m : mon)
    {
        if (!m->is_defeated())
        {
            ans.push_back(m);
        }
    }

    if (ans.empty())
    {
        cout << "All monsters are defeated.\n";
        boundary();
        return nullptr;
    }

    while (true)
    {
        cout << "Available Monsters:\n";
        for (size_t i = 0; i < ans.size(); ++i)
        {
            cout << "  " << i + 1 << ". " << ans[i]->get_name() << '\n';
        }

        cout << "\nChoose one of the monsters by number: ";
        string input;
        getline(cin, input);

        int index = -1;
        try
        {
            size_t pos;
            index = stoi(input, &pos);
            if (pos != input.size() || index < 1 || index > (int)ans.size())
            {
                throw invalid_argument("Out of range");
            }

            boundary();
            return ans[index - 1];
        }
        catch (...)
        {
            cout << "Invalid input! Please enter a valid number between 1 and " << ans.size() << ".\n";
        }
    }
}

void Game::heroPhase(Hero *hero)
{
    bool endPhase = false;
    hero->resetActions();

    while (hero->hasActionsLeft() && !endPhase)
    {
        string Choice = actionMenu();
        try
        {
            if (Choice == "M")
            {
                // مکان های مجاور رو به کاربر نشون میده
                Location *dest = askLocationChoice(getCurrentHero()->getLocation()->get_neighbors());
                if (dest)
                {
                    // انتخاب از بین محلی های خانه فعلی
                    vector<Villager *> toMove = askVillagerChoice(getCurrentHero()->getLocation()->get_villagers());
                    hero->move(dest, toMove);
                }
            }
            else if (Choice == "G")
            {
                // انتخاب محلی
                vector<Villager *> v = askVillagerChoice(getCurrentHero()->getLocation()->getNearByVillagers());
                if (v.empty())
                {
                    continue;
                }
                if (v.size() != 1)
                {
                    cout << "You should select only one villager!\n";
                    continue;
                }
                // انتخاب مقصد مجاور
                if (v[0]->getCurrentLocation()->isNeighbor(hero->getLocation()))
                { // بررسی مجاور بودن محلی نسبت به قهرمان
                    hero->guide(v, hero->getLocation());
                }
                else
                {
                    Location *dest = askLocationChoice(getCurrentHero()->getLocation()->get_neighbors());
                    if (dest && !v.empty())
                    {
                        hero->guide(v, dest);
                    }
                }
            }
            else if (Choice == "P")
            {
                auto items = askItemSelection(getCurrentHero()->getLocation()->get_items());
                if (!items.empty())
                {
                    hero->pickUp(items);
                }
            }
            else if (Choice == "A")
            {
                auto *whichMonster = askMonsterChoice(monsters);
                if (whichMonster)
                {
                    hero->advanced(whichMonster);
                }
            }
            else if (Choice == "D")
            {
                auto *whichMonster = askMonsterChoice(monsters);
                if (whichMonster)
                {
                    hero->defeat(whichMonster);
                    //  حذف از مکان فعلی
                    if (hero->getLocation())
                    {
                        hero->getLocation()->removeMonster(whichMonster);
                        whichMonster->set_location(nullptr);
                    }
                }
            }
            else if (Choice == "S")
            {
                if (auto *arch = dynamic_cast<Archaeologist *>(hero))
                {
                    Location *selectLocation = askLocationChoice(getCurrentHero()->getLocation()->get_neighbors());
                    auto neiborItems = selectLocation->get_items();
                    arch->specialAction(neiborItems);
                }
                else
                {
                    cout << "This hero doesn't have a special action.\n";
                }
            }
            else if (Choice == "Q")
            {
                cout << "Quitting...\n";
                exit(0);
            }
            else if (Choice == "H")
            {
                cout << "\n Available actions:\n";
                cout << "  M - Move\n";
                cout << "  G - Guide\n";
                cout << "  P - Pick Up\n";
                cout << "  A - Advance\n";
                cout << "  D - Defeat\n";
                cout << "  S - Special Action\n";
                cout << "  U - Use Perk\n";
                cout << "  Q - Quit\n";
                cout << "  H - Help\n";

                cout << "\nEnter the letter of the action you want help with: ";
                string helpChoice;
                cin >> helpChoice;
                cin.clear();
                cin.ignore(1000, '\n');

                if (helpChoice == "M")
                {
                    cout << "Move: Move to one of the neighboring locations.\n";
                }
                else if (helpChoice == "G")
                {
                    cout << "Guide: Guide one or more villagers to a neighboring location.\n";
                }
                else if (helpChoice == "P")
                {
                    cout << "Pick Up: Pick up items from your current location.\n";
                }
                else if (helpChoice == "A")
                {
                    cout << "Advance: Attempt to progress a monster's mission if you're in a valid location and have required items.\n";
                }
                else if (helpChoice == "D")
                {
                    cout << "Defeat: Try to defeat a monster if its mission is complete and you're in the same location.\n";
                }
                else if (helpChoice == "S")
                {
                    cout << "Special Action: Use your hero's unique ability (if available).\n";
                }
                else if (helpChoice == "U")
                {
                    cout << "Use Perk: Use one of your Perk cards. This does not cost an action.\n";
                }
                else if (helpChoice == "Q")
                {
                    cout << "Quit: Exit the game.\n";
                }
                else if (helpChoice == "H")
                {
                    cout << "Help: Show this help menu.\n";
                }
                else
                {
                    cout << "Invalid help option selected.\n";
                }
            }
            else if (Choice == "U")
            { // کارت پرک بدون کم کردن اکشن اجرا میشه
                usePerkCard();
            }
            else
            {
                throw invalid_argument("Invalid input!\n");
            }
        }
        catch (const std::exception &e)
        {
            cout << e.what();
        }
        rescueVillagerIfSafe(getCurrentHero()); // بررسی نجات محلی ها بعد از هر اکشن
    }
}

int Game::askPerkCardChoice(Hero *hero)
{
    const auto &perkCards = hero->getPerkCard();

    if (perkCards.empty())
    {
        cout << "You don't have any perk cards!\n";
        return -1;
    }

    while (true)
    {
        cout << "Choose one of your perk cards by number:\n";
        cout << hero->getPerkSummary();

        string input;
        getline(cin, input);

        int index = -1;
        try
        {
            size_t pos;
            index = stoi(input, &pos);
            if (pos != input.size() || index < 1 || index > (int)perkCards.size())
            {
                throw invalid_argument("Out of range");
            }

            boundary();
            return index - 1;
        }
        catch (...)
        {
            cout << "Invalid input! Please enter a valid number between 1 and " << perkCards.size() << ".\n";
        }
    }
}

void Game::usePerkCard()
{
    Hero *hero = getCurrentHero();

    int index = askPerkCardChoice(hero);

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
            Location *loc = askLocationChoice(map->getAllLocations());
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
                cout << "Invisible Man was placed in your closen location.\n";
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
            cout << "Monster phase skipped, 2 items placed on the map.\n";
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

            cout << "Each hero put 1 item on the map.\n";
            break;
        }
        case PerkType::LATE_INTO_THE_NIGHT:
        {
            hero->addAction(2);
            cout << "You gained 2 extra actions!\n";
            break;
        }
        case PerkType::REPEL:
        {
            Monster *mon = askMonsterChoice(monsters);
            if (!mon)
            {
                cout << "No monster selected.\n";
                break;
            }

            Location *current = mon->get_location();

            //  اولین قدم: انتخاب اولین خانه
            Location *step1 = askLocationChoice(current->get_neighbors());
            if (!step1)
            { // هیولا رو اصلا حرکت ندادیم
                cout << "Monster did not move.\n";
                break;
            }

            //  دومین قدم: انتخاب دومین خانه
            Location *step2 = askLocationChoice(step1->get_neighbors());

            Location *finalLoc = step2 ? step2 : step1; // بررسی یک یا دو حرکت بودن

            //  انتقال هیولا
            Location *oldLoc = mon->get_location();
            if (oldLoc && oldLoc != finalLoc)
            {
                oldLoc->removeMonster(mon);
            }
            finalLoc->addMonster(mon);
            cout << "Monster repelled to " + finalLoc->get_name() + ".\n";
            break;
        }
        case PerkType::HURRY:
        {
            //  قهرمانی که باید حرکت کنه انتخاب بشه
            Hero *selectedHero = askHeroChoice();
            if (!selectedHero)
            {
                cout << "No hero selected.\n";
                break;
            }

            Location *current = selectedHero->getLocation();

            // یک خانه حرکت
            Location *step1 = askLocationChoice(current->get_neighbors());
            if (!step1)
            {
                cout << "Hero stayed in place.";
                break;
            }

            // دو خانه حرکت
            Location *step2 = askLocationChoice(step1->get_neighbors());

            Location *finalLoc = step2 ? step2 : step1;

            //  جابه‌جایی قهرمان
            Location *oldLoc = selectedHero->getLocation();
            if (oldLoc && oldLoc != finalLoc)
            {
                oldLoc->removeHero(selectedHero);
            }
            finalLoc->addHero(selectedHero);

            cout << selectedHero->getName() << " moved to " << finalLoc->get_name() + ".\n";

            break;
        }
        }
    }
    catch (const exception &e)
    {
        cout << e.what();
    }
}

Hero *Game::askHeroChoice()
{
    if (heroes.size() < 2)
    {
        cout << "Not enough heroes available.\n";
        return nullptr;
    }

    while (true)
    {
        cout << "Choose your hero:\n";
        cout << "1. Mayor\n";
        cout << "2. Archaeologist\n";
        cout << "Enter number: ";

        string input;
        getline(cin, input);

        if (input == "1" || input == "2")
        {
            string target = (input == "1") ? "Mayor" : "Archaeologist";

            for (Hero *h : heroes)
            {
                if (h->getClassName() == target)
                {
                    return h;
                }
            }

            cout << "Hero not found in the list! Check hero initialization.\n";
            return nullptr;
        }

        cout << "Invalid input! Please enter 1 or 2.\n";
    }
}

void Game::monsterPhase()
{
    int invisibleManPowerCount = 0;
    Invisible_Man *invisibleMan = nullptr;
    map = Map::get_instanse();
    ItemManager &itemManager = ItemManager::getInstance();

    //  بررسی پرک کارت برای ادامه فاز هیولا
    if (skipNextMonsterPhase)
    {
        skipNextMonsterPhase = false;
        cout << "Monster Phase skipped due to perk card.\n";
        return;
    }

    // بررسی خالی بودن کارت‌ها
    if (monsterDeck.empty())
    {
        cout << "No monster cards left.\n";
        return;
    }

    //  کشیدن کارت
    MonsterCard card = monsterDeck.back();
    monsterDeck.pop_back();
    cout << "Monster Card: " << card.get_name() << '\n';
    cout << "\tCard's Event: " << card.get_event() << '\n';

    // قرار دادن آیتم‌ها روی نقشه
    for (int i = 0; i < card.get_useItem(); ++i)
    {
        Item *item = itemManager.getRandomItem();
        if (item && item->get_location())
        {
            item->get_location()->addItem(item);
        }
    }

    //  اجرای event
    useMonsterCard(card.get_name());

    bool CONTINUE = true;
    int movement = card.get_numMove(); // تعداد حرکت
    vector<string> strike = card.get_strkTrgt();
    Monster *targetMonster = nullptr;

    for (size_t j = 0; j < strike.size() && CONTINUE; j++)
    {
        if (strike[j] == "Frenzied Monster")
        {
            targetMonster = frenzy;
            Location *prev = frenzy->get_location();
            frenzy->moveTowardTarget(heroes, villagers, movement);
            Location *current = frenzy->get_location();

            if (prev != current)
            {
                cout << "Frenzy Monster (" << frenzy->get_name() << ") moved to " << current->get_name() << '\n';
            }
            else
            {
                cout << "Frenzy Monster (" << frenzy->get_name() << ") stayed in place." << '\n';
            }
        }
        else
        {
            for (Monster *mon : monsters)
            {
                if (mon->get_name() == strike[j])
                {
                    targetMonster = mon;
                    Location *prev = mon->get_location();
                    mon->moveTowardTarget(heroes, villagers, movement);
                    Location *current = mon->get_location();

                    if (prev != current)
                    {
                        cout << mon->get_name() << " moved to " << current->get_name() << '\n';
                    }
                    else
                    {
                        cout << mon->get_name() << " stayed in place." << '\n';
                    }
                    break;
                }
            }
        }
        for (int i = 0; i < card.get_diceCount() && CONTINUE; ++i)
        {
            Location *current = targetMonster->get_location();

            Face face = card.rollDie();
            cout << "Dice for " << targetMonster->get_name() + ": " << card.faceToString(face) << '\n';

            if (face == Face::STRIKE)
            {
                Hero *targetHero = nullptr;
                Villager *targetVillager = nullptr;

                // فقط اولین هیرویی که در این مکان قرار دارد انتخاب می‌شود
                for (Hero *h : heroes)
                {
                    if (h->getLocation() == current)
                    {
                        targetHero = h;
                        break;
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

                // اگر هیرو هدف پیدا شد، حمله انجام می‌شود
                if (targetHero)
                {
                    vector<Item *> items = targetHero->getItems();
                    if (!items.empty())
                    {
                        while (true)
                        {
                            Item *choice = askItemToDefend(items);
                            if (!choice)
                            {
                                terrorLevel++;
                                targetHero->getLocation()->removeHero(targetHero); // حذف قهرمان از لوکیشن قبلی
                                map->getLocation("Hospital")->addHero(targetHero); // اضافه کردن قهرمان به لوکیشن فعلی
                                cout << targetHero->getName() << " took damage and was sent to the Hospital!\n";
                                break;
                            }
                            else
                            {
                                targetHero->removeItem(choice);
                                itemManager.recycleItemToUsedItems(choice);
                                cout << targetHero->getName() << " used " << choice->get_name() << " to block the attack!\n";
                                break;
                            }
                        }
                    }
                    else
                    {
                        terrorLevel++;
                        targetHero->getLocation()->removeHero(targetHero);
                        map->getLocation("Hospital")->addHero(targetHero);
                        cout << targetHero->getName() << " had no items and was sent to the Hospital!\n";
                    }

                    CONTINUE = false; //  برای پایان فاز مانستر
                }
                //  اگر هیرو در مکان نبود، بررسی حمله به ویلیجر
                else if (targetVillager)
                {
                    targetVillager->kill();
                    terrorLevel++;
                    cout << targetVillager->getName() << " was killed by " << targetMonster->get_name() << "!\n";
                    targetVillager->getCurrentLocation()->removeVillager(targetVillager);
                    targetVillager->setLocation(nullptr);
                    CONTINUE = false;
                }
            }
            else if (face == Face::POWER)
            {
                cout << targetMonster->get_name() << " activates their special power.\n";
                if (targetMonster->get_name() == "Invisible Man")
                {
                    invisibleManPowerCount++;
                    invisibleMan = dynamic_cast<Invisible_Man *>(targetMonster);
                }
                else
                {
                    targetMonster->specialPower(getCurrentHero());
                }
            }
            // EMPTY : هیچ اتفاقی نمی‌افتد
        }
        cout << "^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n";
    }
    if (invisibleMan != nullptr && invisibleManPowerCount > 0)
    {
        for (int i = 0; i < invisibleManPowerCount; i++)
        {
            invisibleMan->specialPower(getCurrentHero());
        }
    }
}

void Game::useMonsterCard(const string &NAME)
{
    map = Map::get_instanse();

    try
    {
        if (NAME == "Form of The Bat")
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

                cout << "Dracula has flown to " << heroLoc->get_name() << "!\n";
            }
            else
            {
                cout << "Could not move Dracula. Hero or Dracula missing.\n";
            }
        }
        else if (NAME == "Thief")
        {
            ItemManager &itemManager = ItemManager::getInstance();

            //  پیدا کردن Invisible Man
            Monster *invisible = nullptr;
            for (Monster *m : monsters)
            {
                if (m->get_name() == "Invisible Man")
                {
                    invisible = m;
                    break;
                }
            }

            if (!invisible)
            {
                cout << "Invisible Man not found!\n";
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
                cout << "No items found on map to steal!\n";
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

            cout << "Invisible Man moved to " << targetLoc->get_name() << " and stole all " << maxCount << " items!\n";
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
                cout << "Dracula not found!\n";
                return;
            }

            // پیدا کردن مکان Crypt
            Location *crypt = map->getLocation("Crypt");
            if (!crypt)
            {
                cout << "Crypt location not found!\n";
                return;
            }

            //  انتقال Dracula به Crypt
            Location *currentLoc = dracula->get_location();
            if (currentLoc && currentLoc != crypt)
            {
                currentLoc->removeMonster(dracula);
            }

            crypt->addMonster(dracula);

            cout << "Dracula retreated to the Crypt as the sun rises.\n";
        }
        else if (NAME == "The Delivery")
        {
            Location *docks = map->getLocation("Docks");
            if (!docks)
            {
                cout << "Location 'Docks' not found!\n";
                return;
            }

            villagers.push_back(new Villager("Wilbur & Chick", nullptr, map->getLocation("Dungeon")));

            // اضافه کردن به مکان Docks
            docks->addVillager(villagers.back());

            cout << "Wilbur & Chick have arrived at the Docks!\n";
        }
        else if (NAME == "Fortune Teller")
        {
            Location *camp = map->getLocation("Camp");
            if (!camp)
            {
                cout << "Location 'Camp' not found!\n";
                return;
            }

            villagers.push_back(new Villager("Maleva", nullptr, map->getLocation("Shop")));

            // اضافه کردن به مکان camp
            camp->addVillager(villagers.back());

            cout << "Maleva have arrived at the Camp!\n";
        }
        else if (NAME == "Former Employer")
        {
            Location *lab = map->getLocation("Laboratory");
            if (!lab)
            {
                cout << "Location 'Laboratory' not found!\n";
                return;
            }

            villagers.push_back(new Villager("Dr. Cranley", nullptr, map->getLocation("Precinct")));

            lab->addVillager(villagers.back());

            cout << "Dr. Cranley have arrived at the Laboratory!\n";
        }
        else if (NAME == "Hurried Assistant")
        {
            Location *tower = map->getLocation("Tower");
            if (!tower)
            {
                cout << "Location 'Tower' not found!\n";
                return;
            }

            villagers.push_back(new Villager("Fritz", nullptr, map->getLocation("Institute")));

            tower->addVillager(villagers.back());

            cout << "Fritz have arrived at the Tower!\n";
        }
        else if (NAME == "The Innocent")
        {
            Location *barn = map->getLocation("Barn");
            if (!barn)
            {
                cout << "Location 'Barn' not found!\n";
                return;
            }

            villagers.push_back(new Villager("Maria", nullptr, map->getLocation("Camp")));

            barn->addVillager(villagers.back());

            cout << "Maria have arrived at the Barn!\n";
        }
        else if (NAME == "Egyption Expert")
        {
            Location *cave = map->getLocation("Cave");
            if (!cave)
            {
                cout << "Location 'Cave' not found!\n";
                return;
            }

            villagers.push_back(new Villager("Prof. Pearson", nullptr, map->getLocation("Museum")));

            cave->addVillager(villagers.back());

            cout << "Prof. Pearson have arrived at the Cave!\n";
        }
        else if (NAME == "The Ichthyologist")
        {
            Location *ins = map->getLocation("Institute");
            if (!ins)
            {
                cout << "Location 'Institute' not found!\n";
                return;
            }

            villagers.push_back(new Villager("Dr. Reed", nullptr, map->getLocation("Camp")));

            ins->addVillager(villagers.back());

            cout << "Dr. Reed have arrived at the Institute!\n";
        }
        else if (NAME == "Hypnotic Gaze")
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
                cout << "Dracula not found!\n";
                return;
            }

            Location *draculaLoc = dracula->get_location();
            if (!draculaLoc)
            {
                cout << "Dracula has no location!\n";
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
                if (path.size() >= 2)
                {
                    Location *next = path[1];
                    current->removeHero(closesHero);
                    next->addHero(closesHero);
                    cout << closesHero->getName() << " was drawn 1 space closer to dracula.\n";
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
                    cout << closesVillager->getName() << " was drawn 1 space closer to dracula.\n";
                }
            }
        }
        else if (NAME == "On The Move")
        {
            updateFrenzy();
            cout << "Frenzy marker moved to: " << frenzy->get_name() << '\n';

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
                    cout << v->getName() << " moved 1 step toward their safe location.\n";
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