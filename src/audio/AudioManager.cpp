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
    backgroundMusic = LoadMusicStream("assets/audio/music/main_music.mp3");
    soundsEffects[""] = LoadSound("assets/audio/sounds/click.mp3");
    SetMusicVolume(backgroundMusic, musicVolume);
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
    SetMusicVolume(backgroundMusic, volume);
}

void AudioManager::setSoundVolume(float volume)
{
    sounDVolume = volume;
}

void AudioManager::dispose()
{
    UnloadMusicStream(introMusic);
    UnloadMusicStream(backgroundMusic);
    for (auto &[name, sound] : soundsEffects)
    {
        UnloadSound(sound);
    }
    soundsEffects.clear();
    CloseAudioDevice();
}