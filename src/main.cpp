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

void menuHero()
{
    cout << "\n*****************************************\n";
    cout << "*\t" << "1.Mayor                         *\n";
    cout << "*\t" << "2.Archaeologist                 *\n";
    cout << "*****************************************\n";
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
    while (true)
    {
        if (game.getTime1() <= game.getTime2())
        {
            cout << "Player " << game.getName1() << " choose your hero:\t";
            menuHero();
            string choice;
            getline(cin, choice);
            if (choice == "1")
            {
                game.assignHeroes(game.getName1(), "Mayor", "Archaeologist");
                return game.getName1();
            }
            else if (choice == "2")
            {
                game.assignHeroes(game.getName1(), "Archaeologist", "Mayor");
                return game.getName1();
            }
        }
        else
        {
            cout << "Player " << game.getName2() << " choose your hero:\t";
            menuHero();
            string choice;
            getline(cin, choice);
            if (choice == "1")
            {
                game.assignHeroes(game.getName2(), "Mayor", "Archaeologist");
                return game.getName2();
            }
            else if (choice == "2")
            {
                game.assignHeroes(game.getName2(), "Archaeologist", "Mayor");
                return game.getName2();
            }
        }
        cout << "Invalid input!\n";
    }
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