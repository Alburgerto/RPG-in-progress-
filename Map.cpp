#include "Map.h"
#include "StateManager.h"

Map::Map(SharedContext* l_context, BaseState* l_currentState) : m_context(l_context), m_defaultTile(l_context), m_mapSize(32, 32),
																m_tileCount(0), m_tileSetCount(0), m_gravity(512.f), m_loadNextMap(false),
																m_currentState(l_currentState) {
	m_context->m_map = this;
	loadTiles("Assets/Tiles.cfg.txt");
}

Map::~Map() {
	purgeMap();
	purgeTileSet();
	m_context->m_map = nullptr;
}


Tile* Map::getTile(unsigned int l_x, unsigned int l_y) {
	auto tile = m_tileMap.find(convertCoords(l_x, l_y));
	return (tile != m_tileMap.end() ? tile->second : nullptr);
}

TileInfo* Map::getDefaultTile() { return &m_defaultTile; }

float Map::getGravity() const { return m_gravity; }

unsigned int Map::getTileSize() const { return (unsigned)Sheet::Tile_Size; }

const sf::Vector2u& Map::getMapSize() const { return m_mapSize; }

const sf::Vector2f& Map::getPlayerStart() const { return m_playerStartPos; }

void Map::loadNext() {
	m_loadNextMap = true;
}

void Map::draw() {
	sf::RenderWindow* l_wind = m_context->m_wind->getRenderWindow();
	l_wind->draw(m_background);
	sf::FloatRect viewSpace = m_context->m_wind->getViewSpace();

	sf::Vector2i tileBegin(floor(viewSpace.left / Sheet::Tile_Size), floor(viewSpace.top / Sheet::Tile_Size)); // Top left tile in view
	sf::Vector2i tileEnd(ceil((viewSpace.left + viewSpace.width) / Sheet::Tile_Size), // Bottom right tile in view
						 ceil((viewSpace.top + viewSpace.height) / Sheet::Tile_Size));

	// Draw only tiles on view
	unsigned int count = 0;
	for (int x = tileBegin.x; x <= tileEnd.x; ++x) {
		for (int y = tileBegin.y; y <= tileEnd.y; ++y) {
			if (x < 0 || y < 0) { continue; }
			Tile* tile = getTile(x, y);
			if (!tile) { continue; }
			sf::Sprite& sprite = tile->m_properties->m_sprite;
			sprite.setPosition(x * Sheet::Tile_Size, y * Sheet::Tile_Size);
			l_wind->draw(sprite);
			++count;

			// Debug.
	/*		if (m_context->m_debugOverlay.Debug()) {
				if (tile->m_properties->m_deadly || tile->m_warp) {
					sf::RectangleShape* tileMarker = new sf::RectangleShape(
						sf::Vector2f(Sheet::Tile_Size, Sheet::Tile_Size));
					tileMarker->setPosition(x * Sheet::Tile_Size, y * Sheet::Tile_Size);
					if (tile->m_properties->m_deadly) {
						tileMarker->setFillColor(sf::Color(255, 0, 0, 100));
					}
					else if (tile->m_warp) {
						tileMarker->setFillColor(sf::Color(0, 255, 0, 150));
					}
					m_context->m_debugOverlay.Add(tileMarker);
				}
			} */
			// End debug.
		}
	}
}

void Map::update(float l_time) {
	if (m_loadNextMap) {
		purgeMap();
		m_loadNextMap = false;
		if (m_nextMap != "") {
			loadMap("Assets/Maps/" + m_nextMap);
		}
		else {
			m_currentState->getStateManager()->switchTo(StateType::GameOver);
		}
		m_nextMap = "";
	}
	sf::FloatRect viewSpace = m_context->m_wind->getViewSpace();
	m_background.setPosition(viewSpace.left, viewSpace.top);
}

// Converts 2D coordinates to 1D ints
unsigned int Map::convertCoords(unsigned int l_x, unsigned int l_y) {
	return (l_x * m_mapSize.x) + l_y;
}

void Map::loadMap(const std::string& l_path) {
	std::ifstream file;
	file.open(l_path);
	if (!file.is_open()) {
		std::cout << "Couldn't open " << l_path << std::endl;
	}
	std::string buffer;
	while (std::getline(file, buffer)) {
		if (buffer[0] == '|') { continue; }
		std::stringstream sstream(buffer);
		std::string type;
		sstream >> type;
		if (type == "BACKGROUND") {
			if (m_backgroundTexture == "") { continue; }
			sstream >> m_backgroundTexture;
			if (!m_context->m_textureManager->requireResource(m_backgroundTexture)) {
				std::cout << "Couldn't load background" << std::endl;
				m_backgroundTexture = "";
				continue;
			}
			sf::Texture* bgTexture = m_context->m_textureManager->getResource(m_backgroundTexture);
			m_background.setTexture(*bgTexture);
			sf::Vector2f viewSize = m_currentState->getView().getSize();
			sf::Vector2u textureSize = bgTexture->getSize();
			sf::Vector2f scaleFactor(viewSize.x / textureSize.x, viewSize.y / textureSize.y);
			m_background.setScale(scaleFactor);
		}
		else if (type == "SIZE") {
			sstream >> m_mapSize.x >> m_mapSize.y;
		}
		else if (type == "GRAVITY") {
			sstream >> m_gravity;
		}
		else if (type == "DEFAULT_FRICTION") {
			sstream >> m_defaultTile.m_friction.x >> m_defaultTile.m_friction.y;
		}
		else if (type == "NEXTMAP") {
			sstream >> m_nextMap;
		}
		else if (type == "PLAYER") {
			sstream >> m_playerStartPos.x >> m_playerStartPos.y;
		}
/*		else if (type == "ENEMY") {

		} */
		else if (type == "TILE") {
			int id;
			sstream >> id;
			auto itr = m_tileSet.find(id);
			if (itr == m_tileSet.end()) { std::cout << "Tile type not registered: " << id << std::endl; continue; }

			Tile* tile = new Tile();
			tile->m_properties = itr->second;
			sf::Vector2u tileCoords;
			sstream >> tileCoords.x >> tileCoords.y;
			if (!m_tileMap.emplace(convertCoords(tileCoords.x, tileCoords.y), tile).second) {
				std::cout << "Duplicate tile" << std::endl;
				delete tile;
				tile = nullptr;
				continue;
			}
			std::string warp;
			sstream >> warp;
			tile->m_warp = false;
			if (warp == "WARP") {
				tile->m_warp = true;
			}
		}
		else {
			std::cout << "Unknown type " << type << std::endl;
		}
	}
}

void Map::loadTiles(const std::string& l_path) {
	std::ifstream file;
	file.open(l_path);
	if (!file.is_open()) {
		std::cerr << "Couldn't open " << l_path << std::endl;
	}
	std::string buffer;
	while (std::getline(file, buffer)) {
		if (buffer[0] == '|') { continue; }
		std::stringstream sstream(buffer);
		int id;
		sstream >> id;
		if (id < 0) { continue; }
		TileInfo* tileInfo = new TileInfo(m_context, "Assets/Textures/tilesheet.png", id);
		sstream >> tileInfo->m_name >> tileInfo->m_friction.x >> tileInfo->m_friction.y >> tileInfo->m_deadly;
		if (!m_tileSet.emplace(tileInfo, id).second) {
			std::cout << "! Duplicate tile type: " << tileInfo->m_name << std::endl;
			delete tileInfo;
		}
	}
	file.close();
}

void Map::purgeMap() {
	m_tileCount = 0;
	for (auto &itr : m_tileMap) {
		delete itr.second;
	}
	m_tileMap.clear();
	m_context->m_entityManager->Purge();

	if (m_backgroundTexture == "") { return; }
	m_context->m_textureManager->releaseResource(m_backgroundTexture);
	m_backgroundTexture = "";
}

void Map::purgeTileSet() {
	for (auto &tile : m_tileSet) {
		delete tile.second;
	}
	m_tileSet.clear();
	m_tileSetCount = 0;
}