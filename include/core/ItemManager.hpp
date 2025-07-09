#ifndef ITEMMANAGER_HPP
#define ITEMMANAGER_HPP

#include <vector>
#include <string>
#include <cstdlib>
#include "core/Item.hpp"

class ItemManager
{
private:
    std::vector<Item *> bag; // آیتم های حاضر در جریان بازی
    std::vector<Item *> usedItems; // آیتم های مصرف شده
    ItemManager();
    ItemManager(const ItemManager &) = delete;
    ItemManager &operator=(const ItemManager &) = delete;

public:
    static ItemManager &getInstance();
    ~ItemManager();
    void initializeItems(); // ساخت آیتم ها در شروع بازی
    Item *getRandomItem(); 
    void recycleItemToUsedItems(Item *); // ذخیره آیتم مصرف شده در خارج از بازی
    int ramainingItems() const; // تعداد آیتم های باقی مانده در bag
    void recycleUsedItemsToBag(); // برگرداندن آیتم هااز خارج بازی به داخل(bag)
    void clear(); 
};

#endif