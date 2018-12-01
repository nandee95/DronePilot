#pragma once

#include <SFML/Graphics.hpp>
#include <map>

class ResourceManager
{
	static std::map<std::string, sf::Font> fonts;
	static std::map<std::string, sf::Texture> textures;
public:

	static const sf::Font* LoadFont(std::string filename)
	{
		auto it = fonts.find(filename);
		if (it != fonts.end())
		{
			return &it->second;
		}
		fonts.insert(std::make_pair(filename, sf::Font()));
		fonts[filename].loadFromFile(filename);
		return &fonts[filename];
	}
	static const sf::Texture* LoadTexture(std::string filename)
	{
		auto it = textures.find(filename);
		if (it != textures.end())
		{
			return &it->second;
		}
		textures.insert(std::make_pair(filename, sf::Texture()));
		textures[filename].loadFromFile(filename);
		return &textures[filename];
	}
};

std::map<std::string, sf::Font> ResourceManager::fonts;
std::map<std::string, sf::Texture> ResourceManager::textures;