#ifndef HERO_HPP
#define HERO_HPP

#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>
#include "Item.hpp"
#include "GameException.hpp"
#include "Villager.hpp"
#include "Location.hpp"
#include "ItemManager.hpp"
#include "PerkCard.hpp"

class Monster;

class Hero
{
protected:
    std::string name;
    Location *location;
    int maxActions;
    int actionsLeft; // تعداد اکشن های باقی مانده
    std::vector<Item *> items;
    std::vector<PerkCard> perkcards;

private:
    bool hasRequiredItems(const std::vector<Item> &) const; // بررسی وجود آیتم های مدنظر
    void consumeItems(const std::vector<Item> &);           // مصرف آیتم از ایتم های قهرمان

public:
    Hero(const std::string &, Location *, int);
    virtual ~Hero() = default;

    // اکشن ها
    virtual void move(Location *, const std::vector<Villager *> &);
    virtual void pickUp(const std::vector<Item *> &);
    virtual void guide(std::vector<Villager *>, Location *);
    virtual void advanced(Monster *);
    virtual void defeat(Monster *);
    virtual void specialAction(const std::vector<Item *> &) = 0;

    void resetActions(); // پس از هر فاز هیولا تعداد امشن ها reset می شود
    void useAction();
    bool hasActionsLeft() const; // آیا اکشنی برای استفاده مانده یا نه؟

    // توابع get
    std::string getName() const;
    Location *getLocation() const;
    int getActionsLeft() const;
    std::vector<Item *> getItems() const;
    std::vector<PerkCard> getPerkCard() const;
    virtual std::string getClassName() const = 0;
    std::string getItemSummary() const;
    int getMaxActions() const;
    std::string getPerkSummary() const;
    std::vector<std::string> getAction() const;

    // توابع set
    void setLocation(Location *);
    void setPerkCard(const PerkCard &);

    PerkCard usePerkCard(int);
    void addAction(int); // اضافه کردن به تعداد اکشن ها
    void removeItem(Item *); // استفاده از آیتم های قهرمان
};

#endif