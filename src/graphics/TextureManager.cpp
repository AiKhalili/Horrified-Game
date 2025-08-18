#include "graphics/TextureManager.hpp"

TextureManager &TextureManager::getInstance()
{
    static TextureManager instance;
    return instance;
}

Texture2D &TextureManager::getOrLoadTexture(const std::string &name, const std::string &fileName)
{
    auto it = textures.find(name);
    if (it != textures.end())
    {
        return it->second;
    }

    Texture2D tex = LoadTexture(fileName.c_str());

    if (tex.id == 0)
    {
        throw std::runtime_error("Failed to load texture: " + fileName);
    }

    textures[name] = tex;
    return textures[name];
}

void TextureManager::unloadAll()
{
    for (auto &[name, texture] : textures)
    {
        UnloadTexture(texture);
    }
    textures.clear();
}