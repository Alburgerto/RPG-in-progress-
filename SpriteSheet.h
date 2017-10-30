#pragma once
#include <SFML\Graphics.hpp>
#include "TextureManager.h"

class SpriteSheet {
public:
	SpriteSheet(TextureManager* l_textureManager);
	~SpriteSheet();

	bool loadSheet(const std::string& l_path);
	void cropSprite(const sf::IntRect& l_rect);

private:
	sf::Sprite m_sprite;
	TextureManager* m_textureManager;
};