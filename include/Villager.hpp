#ifndef VILLAGER_HPP
#define VILLAGER_HPP

#include <string>
#include "Location.hpp"

class Villager
{
private:
    std::string name;
    Location *currentLocation; // مکان فعلی محلی
    Location *safeLocation; // مکان امن محلی
    bool alive = true; // زنده است یا نه؟
    bool rescue = false; // نجات یافته یا نه؟

public:
    Villager(const std::string &, Location *, Location *);
    std::string getName() const;
    Location *getCurrentLocation() const; // گرفتن مکان فعلی محلی
    Location *getSafeLocation() const;    // گرفتن مکان امن محلی
    void setLocation(Location *);         // تغییر مکان محلی مثلا بعد از Move یا Guide
    bool isSafeLocation() const;
    bool isAlive() const;
    void kill();
    void markRescued();
    bool isRescued() const;
};

#endif