#pragma once
#include "ResourceManager.h"
#include <SFML/Graphics/Texture.hpp>

class TextureManager : public ResourceManager<TextureManager, sf::Texture> {
public:
	TextureManager() : ResourceManager("Assets/textures_paths.cfg.txt") {}

	sf::Texture* load(const std::string& l_path) {
		sf::Texture* texture = new sf::Texture();
		if (!texture->loadFromFile(l_path)) {
			delete texture;
			texture = nullptr;
			std::cout << "Failure loading " << l_path << std::endl;
		}
		return texture;
	}
};