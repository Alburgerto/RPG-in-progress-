#pragma once
#include "SharedContext.h"
#include "BaseState.h"
#include <unordered_map>

enum Sheet { Tile_Size = 32, Sheet_Width = 256, Sheet_Height = 256 };
using TileID = unsigned int;

struct TileInfo {

	TileInfo(SharedContext* l_context, const std::string& l_texture = "", TileID l_id = 0) : m_context(l_context), m_texture(l_texture), m_id(l_id), m_deadly(false) {
		if (l_texture == "") { return; }
		if (!m_context->m_textureManager->requireResource(l_texture)) { return; }
		m_texture = l_texture;
		m_sprite.setTexture(*m_context->m_textureManager->load(m_texture));
		sf::IntRect tileBoundaries(m_id % (Sheet::Sheet_Width / Sheet::Tile_Size) * Sheet::Tile_Size,
								   m_id / (Sheet::Sheet_Height / Sheet::Tile_Size) * Sheet::Tile_Size,
								   Sheet::Tile_Size, 
								   Sheet::Tile_Size);
		m_sprite.setTextureRect(tileBoundaries);
	}

	~TileInfo() {
		if (m_texture == "") { return; }
		m_context->m_textureManager->releaseResource(m_texture);
	}

	sf::Sprite m_sprite;

	TileID m_id;
	std::string m_name;
	sf::Vector2f m_friction;
	bool m_deadly;

	SharedContext* m_context;
	std::string m_texture;
};

struct Tile {
	TileInfo* m_properties;
	bool m_warp; // Loads next level when standing on it?
};

using TileSet = std::unordered_map<TileID, TileInfo*>; // All kinds of different tiles   TileID = id
using TileMap = std::unordered_map<TileID, Tile*>;	   // All tiles displayed on screen. TileID = position

class Map {
public:
	Map(SharedContext* l_context, BaseState* l_currentState);
	~Map();

	Tile* getTile(unsigned int l_x, unsigned int l_y);
	TileInfo* getDefaultTile();
	float getGravity() const;
	unsigned int getTileSize() const;
	const sf::Vector2u& getMapSize() const;
	const sf::Vector2f& getPlayerStart() const;

	void loadMap(const std::string& l_path);
	void loadNext();

	void draw();
	void update(float l_time);
private:
	// Converts 2D coordinates to 1D ints
	unsigned int convertCoords(unsigned int l_x, unsigned int l_y); 
	void loadTiles(const std::string& l_path);
	void purgeMap();
	void purgeTileSet();

	TileSet m_tileSet;
	TileMap m_tileMap;
	TileInfo m_defaultTile;

	std::string m_backgroundTexture;
	sf::Sprite m_background;

	sf::Vector2u m_mapSize;
	sf::Vector2f m_playerStartPos;
	unsigned int m_tileCount;
	unsigned int m_tileSetCount;
	float m_gravity;

	std::string m_nextMap;
	bool m_loadNextMap;

	SharedContext* m_context;
	BaseState* m_currentState;
};