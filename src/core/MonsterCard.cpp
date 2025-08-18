#include "core/MonsterCard.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
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

std::string MonsterCard::serialize() const {
    std::string result = "MonsterCard|";
    result += name + "|";
    result += std::to_string(useItem) + "|";

    std::string safeEvent = event;
    size_t pos = 0;
    while ((pos = safeEvent.find('\n', pos)) != std::string::npos) {
        safeEvent.replace(pos, 1, "\\n");
        pos += 2;
    }
    result += safeEvent + "|";

    result += std::to_string(numMovement) + "|";
    result += std::to_string(diceCount) + "|";

    for (size_t i = 0; i < strikeTargets.size(); ++i) {
        result += strikeTargets[i];
        if (i != strikeTargets.size() - 1)
            result += ",";
    }

    return result;
}


MonsterCard MonsterCard::deserialize(const std::string& line) {
    std::vector<std::string> parts;
    std::string current;
    for (char c : line) {
        if (c == '|') {
            parts.push_back(current);
            current.clear();
        } else {
            current += c;
        }
    }
    parts.push_back(current);

    if (parts.size() < 7) {
        throw std::runtime_error("Invalid MonsterCard format: expected at least 7 fields");
    }

    std::string restoredEvent = parts[3];
    size_t pos = 0;
    while ((pos = restoredEvent.find("\\n", pos)) != std::string::npos) {
        restoredEvent.replace(pos, 2, "\n");
        pos += 1;
    }

    auto safe_stoi = [](const std::string& s, const std::string& field) {
        if (s.empty()) return 0;
        try {
            return std::stoi(s);
        } catch (...) {
            throw std::runtime_error("Invalid " + field + " value: '" + s + "'");
        }
    };

    int useItem = safe_stoi(parts[2], "useItem");
    int numMove = safe_stoi(parts[4], "numMove");
    int diceCount = safe_stoi(parts[5], "diceCount");

    std::vector<std::string> targets;
    std::string targetStr = parts[6];
    std::string currentTarget;
    for (char c : targetStr) {
        if (c == ',') {
            if (!currentTarget.empty()) {
                currentTarget.erase(std::remove_if(currentTarget.begin(), currentTarget.end(), 
                    [](char ch) { return ch == '\n' || ch == '\r'; }), currentTarget.end());
                currentTarget.erase(0, currentTarget.find_first_not_of(" "));
                currentTarget.erase(currentTarget.find_last_not_of(" ") + 1);
                if (!currentTarget.empty())
                    targets.push_back(currentTarget);
                currentTarget.clear();
            }
        } else {
            currentTarget += c;
        }
    }
    if (!currentTarget.empty()) {
        currentTarget.erase(std::remove_if(currentTarget.begin(), currentTarget.end(), 
            [](char ch) { return ch == '\n' || ch == '\r'; }), currentTarget.end());
        currentTarget.erase(0, currentTarget.find_first_not_of(" "));
        currentTarget.erase(currentTarget.find_last_not_of(" ") + 1);
        if (!currentTarget.empty())
            targets.push_back(currentTarget);
    }

    return MonsterCard(
        parts[1],        // name
        useItem,         // useItem
        restoredEvent,   // event
        numMove,         // numMove
        diceCount,       // diceCount
        targets          // strikeTargets
    );
}
