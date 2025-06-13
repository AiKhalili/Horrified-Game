#ifndef ITEMMANAGER_HPP
#define ITEMMANAGER_HPP

#include <vector>
#include <string>
#include <cstdlib>
#include "Item.hpp"

class ItemManager
{
private:
    std::vector<Item *> bag;
    std::vector<Item *> usedItems;
    ItemManager();
    ItemManager(const ItemManager &) = delete;
    ItemManager &operator=(const ItemManager &) = delete;

public:
    static ItemManager &getInstance();
    ~ItemManager();
    void initializeItems();
    Item *getRandomItem();
    void recycleItemToUsedItems(Item *); // ذخیره آیتم مصرف شده در خارج از بازی
    int ramainingItems() const;
    void recycleUsedItemsToBag();
    std::vector<Item*> findItemsAtLocation(const std::vector<std::string>&,int);  
};

#endif