#include "audio/AudioManager.hpp"
#include <iostream>

AudioManager &AudioManager::getInstance()
{
    static AudioManager instance;
    return instance;
}

void AudioManager::init()
{
    InitAudioDevice();
    introMusic = LoadMusicStream("assets/audio/music/intro_theme.mp3");
    SetMusicVolume(introMusic, musicVolume);
    soundsEffects["click"] = LoadSound("assets/audio/sounds/click.mp3");
    soundsEffects["magic"] = LoadSound("assets/audio/sounds/magic.wav");
    soundsEffects["whoosh"] = LoadSound("assets/audio/sounds/whoosh.mp3");
    soundsEffects["flying"] = LoadSound("assets/audio/sounds/flying.mp3");
    soundsEffects["door"] = LoadSound("assets/audio/sounds/door.mp3");
    monsterSelectMusic = LoadMusicStream("assets/audio/music/monster_selection.mp3");
    rescueMusic = LoadMusicStream("assets/audio/music/rescue.mp3");
    winMusic = LoadMusicStream("assets/audio/music/win.mp3");
    loseMusic = LoadMusicStream("assets/audio/music/lose.mp3");
    timeoutMusic = LoadMusicStream("assets/audio/music/timeout.mp3");
    perkMusic = LoadMusicStream("assets/audio/music/Perk.wav");
    AdDefMusic = LoadMusicStream("assets/audio/music/Ad&De.mp3");

    setMusicVolume(musicVolume);
}

void AudioManager::playIntroMusic()
{
    if (!introStarted)
    {
        PlayMusicStream(introMusic);
        introStarted = true;
    }
}

void AudioManager::stopIntroMusic()
{
    StopMusicStream(introMusic);
    introStarted = false;
}

void AudioManager::playMusic()
{
    if (!musicStarted)
    {
        PlayMusicStream(backgroundMusic);
        musicStarted = true;
    }
}

void AudioManager::update()
{
    if (musicStarted)
    {
        UpdateMusicStream(backgroundMusic);
    }
    if (introStarted)
    {
        UpdateMusicStream(introMusic);
    }
    if (monsterSelectStarted)
    {
        UpdateMusicStream(monsterSelectMusic);
    }
    if (rescueStarted)
    {
        UpdateMusicStream(rescueMusic);
    }
    if (winStarted)
    {
        UpdateMusicStream(winMusic);
    }
    if (loseStarted)
    {
        UpdateMusicStream(loseMusic);
    }
    if (timeoutStarted)
    {
        UpdateMusicStream(timeoutMusic);
    }
    if (perkSelectStarted)
    {
        UpdateMusicStream(perkMusic);
    }
    if (AdDefSelectStarted)
    {
        UpdateMusicStream(AdDefMusic);
    }
}

void AudioManager::stopMusic()
{
    StopMusicStream(backgroundMusic);
    musicStarted = false;
}

void AudioManager::playSoundEffect(const std::string &name)
{
    if (soundsEffects.count(name))
    {
        SetSoundVolume(soundsEffects[name], sounDVolume);
        PlaySound(soundsEffects[name]);
    }
    else
    {
        std::cerr << "[AudioManager] sound not found: " << name << '\n';
    }
}

void AudioManager::setMusicVolume(float volume)
{
    musicVolume = volume;
    SetMusicVolume(introMusic, volume);
    SetMusicVolume(backgroundMusic, volume);
    SetMusicVolume(monsterSelectMusic, volume);
    SetMusicVolume(rescueMusic, volume);
    SetMusicVolume(winMusic, volume);
    SetMusicVolume(loseMusic, volume);
    SetMusicVolume(timeoutMusic, volume);
    SetMusicVolume(perkMusic, volume);
    SetMusicVolume(AdDefMusic, volume);
}

void AudioManager::setSoundVolume(float volume)
{
    sounDVolume = volume;
}

void AudioManager::dispose()
{
    UnloadMusicStream(introMusic);
    UnloadMusicStream(backgroundMusic);
    UnloadMusicStream(monsterSelectMusic);
    UnloadMusicStream(rescueMusic);
    UnloadMusicStream(winMusic);
    UnloadMusicStream(loseMusic);
    UnloadMusicStream(timeoutMusic);
    UnloadMusicStream(perkMusic);
    UnloadMusicStream(AdDefMusic);
    for (auto &[name, sound] : soundsEffects)
    {
        UnloadSound(sound);
    }
    soundsEffects.clear();
    CloseAudioDevice();
}

void AudioManager::playMonsterSelectMusic()
{
    StopMusicStream(backgroundMusic);
    musicStarted = false;

    if (!monsterSelectStarted)
    {
        PlayMusicStream(monsterSelectMusic);
        monsterSelectStarted = true;
    }
}

void AudioManager::stopMonsterSelectMusic()
{
    if (monsterSelectStarted)
    {
        StopMusicStream(monsterSelectMusic);
        monsterSelectStarted = false;
    }
    PlayMusicStream(backgroundMusic);
    musicStarted = true;
}

void AudioManager::playRescueMusic()
{
    StopMusicStream(backgroundMusic);
    musicStarted = false;

    if (!rescueStarted)
    {
        PlayMusicStream(rescueMusic);
        rescueStarted = true;
    }
}

void AudioManager::stopRescueMusic()
{
    if (rescueStarted)
    {
        StopMusicStream(rescueMusic);
        rescueStarted = false;
    }
    PlayMusicStream(backgroundMusic);
    musicStarted = true;
}

void AudioManager::playWinMusic()
{
    StopMusicStream(backgroundMusic);
    musicStarted = false;

    if (!winStarted)
    {
        PlayMusicStream(winMusic);
        winStarted = true;
    }
}

void AudioManager::stopWinMusic()
{
    if (winStarted)
    {
        StopMusicStream(winMusic);
        winStarted = false;
    }
    PlayMusicStream(backgroundMusic);
    musicStarted = true;
}

void AudioManager::playLoseMusic()
{
    StopMusicStream(backgroundMusic);
    musicStarted = false;

    if (!loseStarted)
    {
        PlayMusicStream(loseMusic);
        loseStarted = true;
    }
}

void AudioManager::stopLoseMusic()
{
    if (loseStarted)
    {
        StopMusicStream(loseMusic);
        loseStarted = false;
    }
    PlayMusicStream(backgroundMusic);
    musicStarted = true;
}

void AudioManager::playTimeOutMusic()
{
    StopMusicStream(backgroundMusic);
    musicStarted = false;

    if (!timeoutStarted)
    {
        PlayMusicStream(timeoutMusic);
        timeoutStarted = true;
    }
}

void AudioManager::stopTimeOutMusic()
{
    if (timeoutStarted)
    {
        StopMusicStream(timeoutMusic);
        timeoutStarted = false;
    }
    PlayMusicStream(backgroundMusic);
    musicStarted = true;
}

void AudioManager::playAdDefSelectMusic()
{
    StopMusicStream(backgroundMusic);
    musicStarted = false;

    if (!AdDefSelectStarted)
    {
        PlayMusicStream(AdDefMusic);
        AdDefSelectStarted = true;
    }
}

void AudioManager::stopPerkSelectMusic()
{
    if (perkSelectStarted)
    {
        StopMusicStream(perkMusic);
        perkSelectStarted = false;
    }
    PlayMusicStream(backgroundMusic);
    musicStarted = true;
}

void AudioManager::playPerkSelectMusic()
{
    StopMusicStream(backgroundMusic);
    musicStarted = false;

    if (!perkSelectStarted)
    {
        PlayMusicStream(perkMusic);
        perkSelectStarted = true;
    }
}

void AudioManager::stopAdDefSelectMusic()
{
    if (AdDefSelectStarted)
    {
        StopMusicStream(AdDefMusic);
        AdDefSelectStarted = false;
    }
    PlayMusicStream(backgroundMusic);
    musicStarted = true;
}