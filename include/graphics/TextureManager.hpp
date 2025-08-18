#ifndef TEXTUREMANAGER_HPP
#define TEXTUREMANAGER_HPP

#include <unordered_map>
#include <string>
#include <stdexcept>
#include "raylib.h"

class TextureManager
{
private:
    std::unordered_map<std::string, Texture2D> textures;
    TextureManager() = default;

public:
    static TextureManager &getInstance();
    Texture2D &getOrLoadTexture(const std::string &name, const std::string &fileName);
    void unloadAll();
    TextureManager(const TextureManager &) = delete;
    void operator=(const TextureManager) = delete;
};

#endif