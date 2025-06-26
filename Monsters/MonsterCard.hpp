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
    std::string name;
    int useItem;
    std::string event;//رویداد
    int numMovement;//تعداد حرکت هیولا
    int diceCount;// تعداد پرتاپ تاس
    std::vector<std::string> strikeTargets;//ترتیب استرایک ها
    Face face;

    public:
    MonsterCard(std::string , int,std::string,int,int,std::vector<std::string>);

    //توابعget
    int get_useItem() const;
    std::string get_event() const;
    int get_numMove() const;
    std::string get_name() const;
    int get_diceCount() const;
    const std::vector<std::string>& get_strkTrgt() const; 
    Face getFace() const;

    void updateFace();
    Face rollDie() const;
    std::string faceToString(Face f) const;
};
#endif