#include "core/SceneDataHub.hpp"

SceneDataHub &SceneDataHub::getInstance()
{
    static SceneDataHub instance;
    return instance;
}

void SceneDataHub::reset()
{
    monsterPhaseLog.clear();
}

void SceneDataHub::setMonsterPhaseLog(const std::vector<std::string> &log)
{
    monsterPhaseLog = log;
}

std::vector<std::string> SceneDataHub::getMonsterPhaseLog() const
{
    return monsterPhaseLog;
}