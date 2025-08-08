#ifndef AUDIOMANAGER_HPP
#define AUDIOMANAGER_HPP

#include "raylib.h"
#include <string>
#include <unordered_map>

class AudioManager
{
public:
    static AudioManager &getInstance();
    void init();
    void playMusic(); // play the main music just once
    void stopMusic();
    void playIntroMusic();
    void stopIntroMusic();
    void update();
    void playSoundEffect(const std::string &name); // for playing one shot sound
    void setMusicVolume(float volume);
    void setSoundVolume(float volume);
    void dispose(); // Releasing the memory used by sounds and music

    void playMonsterSelectMusic();
    void stopMonsterSelectMusic();

    void playRescueMusic();
    void stopRescueMusic();

    void playWinMusic();
    void stopWinMusic();

    void playLoseMusic();
    void stopLoseMusic();

    void playTimeOutMusic();
    void stopTimeOutMusic();

private:
    AudioManager() = default;
    Music backgroundMusic;
    Music introMusic;
    Music monsterSelectMusic;
    Music rescueMusic;
    Music winMusic;
    Music loseMusic;
    Music timeoutMusic;

    bool monsterSelectStarted = false;
    bool rescueStarted = false;
    bool introStarted = false;
    bool musicStarted = false;
    bool winStarted = false;
    bool loseStarted = false;
    bool timeoutStarted = false;

    std::unordered_map<std::string, Sound> soundsEffects;
    float musicVolume = 0.5f;
    float sounDVolume = 0.5f;
};

#endif