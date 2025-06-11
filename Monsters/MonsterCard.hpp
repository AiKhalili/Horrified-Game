#ifndef MONSTERCARD_HPP
#define MONSTERCARD_HPP
#include <iostream>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <algorithm>

enum class Face { EMPTY, STRIKE, POWER }; 

class MonsterCard{
    private:
    int useItem;
    std::string event;
    int numMovement;
    int diceCount;
    std::vector<std::string> strikeTargets;
    Face face;

    public:
    MonsterCard(int,std::string,int,int,std::vector<std::string>,Face);
    int get_useItem() const;
    std::string get_event() const;
    int get_numMove() const;
    int get_diceCount() const;
    const std::vector<std::string>& get_strkTrgt() const; 
    Face rollDie() const;
    std::string faceToString(Face f) const;
};
#endif