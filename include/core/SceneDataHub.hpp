#ifndef SCENEDATAHUB_HPP
#define SCENEDATAHUB_HPP

#include <vector>
#include <string>

class SceneDataHub
{
private:
    std::vector<std::string> monsterPhaseLog;
    SceneDataHub() = default;

public:
    static SceneDataHub &getInstance();
    SceneDataHub(const SceneDataHub &) = delete;
    SceneDataHub &operator=(const SceneDataHub &) = delete;

    void reset();

    void setMonsterPhaseLog(const std::vector<std::string> &log);

    std::vector<std::string> getMonsterPhaseLog();
};

#endif