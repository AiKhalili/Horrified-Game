#include "graphics/TextureManager.hpp"

TextureManager &TextureManager::getInstance()
{
    static TextureManager instance;
    return instance;
}

void TextureManager::loadTexture(const std::string &name, const std::string &fileName)
{
    if (textures.find(name) != textures.end())
    {
        return;
    }

    Texture2D tex = LoadTexture(fileName.c_str());

    if (tex.id == 0)
    {
        throw std::runtime_error("Failed to load texture: " + fileName);
    }

    textures[name] = tex;
}

Texture2D &TextureManager::getTexture(const std::string &name)
{
    auto it = textures.find(name);
    if (it == textures.end())
    {
        throw std::runtime_error("Texture not found: " + name);
    }
    return it->second;
}

void TextureManager::unloadAll()
{
    for (auto &[name, texture] : textures)
    {
        UnloadTexture(texture);
    }
    textures.clear();
}