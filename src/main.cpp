#include <iostream>
#include <iomanip>
#include <limits>
#include "core/Game.hpp"

using namespace std;

void boundary()
{
    cout << "------------------------------------------------------------------------\n";
}

void showMenu()
{
    cout << "*****************************************\n";
    cout << "*\t" << "Select an option :              *\n";
    cout << "*\t" << "1.Start the game.               *\n";
    cout << "*\t" << "2.Quit.                         *\n";
    cout << "*****************************************\n";
}

void menuHero(const std::vector<std::string> &heroes)
{
    cout << "\n************************************\n";
    for (size_t i = 0; i < heroes.size(); ++i)
    {
        cout << "*\t" << (i + 1) << "." << heroes[i];
        int spaceCount = 25 - static_cast<int>(heroes[i].length());
        for (int j = 0; j < spaceCount; ++j)
            cout << " ";
        cout << "*\n";
    }
    cout << "************************************\n";
}

void getPlayersInput(int playerNum)
{
    string name, time;
    cout << "Player" << playerNum << ", Enter your name:\t";
    getline(cin, name);
    cout << "Player" << playerNum << ", Enter the last time you ate garlic(min) :\t";
    getline(cin, time);
    Game::getInstance().setPlayersTimes(playerNum, name, time);
    boundary();
}

void setupPlayers()
{
    for (int i = 1; i <= 2; ++i)
    {
        while (true)
        {
            try
            {
                getPlayersInput(i);
                break;
            }
            catch (const exception &e)
            {
                cout << e.what() << "Please try again.\n";
            }
        }
    }
}

string whoStart()
{
    Game &game = Game::getInstance();
    vector<string> heroList = {"Mayor", "Archaeologist", "Scientist", "Courier"};
    string firstPlayer, secondPlayer;

    if (game.getTime1() <= game.getTime2())
    {
        firstPlayer = game.getName1();
        secondPlayer = game.getName2();
    }
    else
    {
        firstPlayer = game.getName2();
        secondPlayer = game.getName1();
    }

    string choice;
    int choiceNum;

    while (true)
    { // انتخاب هیرو توسط بازیکن اول
        cout << "Player " << firstPlayer << ", choose your hero:\t";
        menuHero(heroList);
        getline(cin, choice);
        try
        {
            size_t pos = 0;
            choiceNum = stoi(choice, &pos);
            if (choiceNum >= 1 && choiceNum <= heroList.size() && pos == choice.size())
            {
                break;
            }
            else
            {
                cout << "Invalid input! Please choose 1 to 4.\n";
            }
        }
        catch (...)
        {
            cout << "Invalid input! Please enter a number.\n";
        }
    }

    string firstHero = heroList[choiceNum - 1];
    heroList.erase(heroList.begin() + (choiceNum - 1)); //  حذف هیرو انتخاب شده از لیست منو

    while (true)
    { // انتخاب هیرو توسط بازیکن دوم
        cout << "Player " << secondPlayer << ", choose your hero:\t";
        menuHero(heroList); // فقط هیروهای باقیمانده
        getline(cin, choice);
        try
        {
            size_t pos = 0;
            choiceNum = stoi(choice, &pos);
            if (choiceNum >= 1 && choiceNum <= heroList.size() && pos == choice.size())
                break;
            else
                cout << "Invalid input! Please choose a valid number.\n";
        }
        catch (...)
        {
            cout << "Invalid input! Please enter a number.\n";
        }
    }

    string secondHero = heroList[choiceNum - 1];

    // ست کردن هیروها
    game.assignHeroes(firstPlayer, firstHero, secondHero);

    return firstPlayer;
}
void ApplyOption(string choice)
{
    Game &game = Game::getInstance();
    if (choice == "1")
    {
        try
        {
            setupPlayers();
            string starter = whoStart();
            game.setupHeroes();
            game.setStartingPlater(starter);
            game.start();
        }
        catch (const GameException &e)
        {
            cout << "Error :\t" << e.what();
        }
    }
    else if (choice == "2")
    {
        cout << "Hope you enjoy it.\n";
        exit(0);
    }
    else
    {
        cout << "Invalid input!\n";
    }
}

string getMenuChoice()
{
    string choice;
    while (true)
    {
        if (cin >> choice)
        {
            if (choice == "1" || choice == "2")
            {
                cin.ignore();
                return choice;
            }
        }
        cin.clear();
        cin.ignore();
        cout << "Invalid input!\n"
             << "Please enter (1-2).\t";
    }
}

int main()
{

    cout << "*****************************************\n";
    cout << "*      WELCOME TO HORRIFIED GAME !      *\n";
    cout << "*****************************************\n";

    Game &game = Game::getInstance();

    while (true)
    {
        showMenu();
        string choice = getMenuChoice();
        ApplyOption(choice);
        cout << "\nDo you want to play again?(y/n):\t";
        string again;
        getline(cin, again);
        if (again != "y" && again != "Y")
        {
            cout << "\nHope you enjoy it!\n";
            break;
        }
        game.reset();
    }

    return 0;
}