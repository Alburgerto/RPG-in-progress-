#include "SpriteSheet.h"

SpriteSheet::SpriteSheet(TextureManager* l_textureManager) : m_textureManager(l_textureManager) {

}

SpriteSheet::~SpriteSheet() {}

bool SpriteSheet::loadSheet(const std::string& l_path) {
	
}

void SpriteSheet::cropSprite(const sf::IntRect& l_rect) {
	m_sprite.setTextureRect(l_rect);
}