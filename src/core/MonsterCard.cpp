#include "core/MonsterCard.hpp"
#include <iostream>
using namespace std;

MonsterCard::MonsterCard(string name, int useItem, std::string event, int numMovement, int diceCount, std::vector<std::string> strikeTargets)
{
    this->name = name;
    this->useItem = useItem;
    this->event = event;
    this->numMovement = numMovement;
    this->diceCount = diceCount;
    this->strikeTargets = strikeTargets;
    this->face = Face::EMPTY;
}

int MonsterCard::get_useItem() const { return useItem; }
string MonsterCard::get_event() const { return event; }
int MonsterCard::get_numMove() const { return numMovement; }
int MonsterCard::get_diceCount() const { return diceCount; }
const vector<string> &MonsterCard::get_strkTrgt() const { return strikeTargets; }
Face MonsterCard::getFace() const { return face; }
string MonsterCard::get_name() const { return name; }

void MonsterCard::updateFace()
{
    face = rollDie();
}

Face MonsterCard::rollDie() const
{
    int value = rand() % 6;
    if (value < 4)
    {
        return Face::EMPTY;
    }
    else if (value == 4)
    {
        return Face::STRIKE;
    }
    else
    {
        return Face::POWER;
    }
}

string MonsterCard::faceToString(Face f) const
{
    switch (f)
    {
    case Face::EMPTY:
        return "blank";
    case Face::STRIKE:
        return "*";
    case Face::POWER:
        return "!";
    default:
        return "Unknown";
    }
}