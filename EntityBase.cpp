#include "EntityBase.h"
#include "EntityManager.h"
#include "SharedContext.h"
#include "Map.h"

bool sortCollisions(const CollisionElement& l_1, const CollisionElement& l_2)
{
	return l_1.m_area > l_2.m_area;
}

EntityBase::EntityBase(EntityManager* l_entityManager) : m_entityManager(l_entityManager), m_name("BaseEntity"),
														 m_type(EntityType::Base), m_id(0), m_referenceTile(nullptr),
														 m_state(EntityState::Idle), m_collidingOnX(false), m_collidingOnY(false) {}

EntityBase::~EntityBase() {}

const sf::Vector2f& EntityBase::getPosition() const { return m_position; }
const sf::Vector2f& EntityBase::getSize() const		{ return m_size; }
EntityState EntityBase::getState() const			{ return m_state; }
std::string EntityBase::getName() const				{ return m_name; }
unsigned int EntityBase::getId() const				{ return m_id; }
EntityType EntityBase::getType() const				{ return m_type; }

void EntityBase::setPosition(float l_x, float l_y) { 
	m_position = sf::Vector2f(l_x, l_y); 
	updateAABB(); 
}

void EntityBase::setPosition(const sf::Vector2f& l_pos) { 
	m_position = l_pos; 
	updateAABB(); 
}

void EntityBase::setSize(float l_x, float l_y) { 
	m_size = sf::Vector2f(l_x, l_y); 
	updateAABB(); 
}

void EntityBase::setState(const EntityState& l_state) {
	if (m_state == EntityState::Dying) { return; }
	m_state = l_state;
}

void EntityBase::move(float l_x, float l_y) {
	m_positionOld = m_position;
	m_position += sf::Vector2f(l_x, l_y);

	Map* map = m_entityManager->getContext()->m_map);
	if (m_position.x < 0) {
		m_position.x = 0;
	}
	else if (m_position.x > map->getMapSize().x * map->getTileSize()) {
		m_position.x = (map->getMapSize().x + 1) * map->getTileSize();
	}

	if (m_position.y < 0) {
		m_position.y = 0;
	}
	else if (m_position.y > (map->getMapSize().y + 1) * map->getTileSize()) {
		m_position.y = (map->getMapSize().y + 1) * map->getTileSize();
		setState(EntityState::Dying);
	}
	updateAABB();
}

void EntityBase::addVelocity(float l_x, float l_y) {
	m_velocity += sf::Vector2f(l_x, l_y);
	if (abs(m_velocity.x > m_maxVelocity.x)) { 
		if (m_velocity.x > 0) {
			m_velocity.x = m_maxVelocity.x;
		}
		else {
			m_velocity.x = -m_maxVelocity.x;
		}
	}
	if (abs(m_velocity.y > m_maxVelocity.y)) { 
		if (m_velocity.y > 0) {
			m_velocity.y = m_maxVelocity.y;
		}
		else {
			m_velocity.y = -m_maxVelocity.y;
		}
	}
}

void EntityBase::accelerate(float l_x, float l_y) {
	m_acceleration += sf::Vector2f(l_x, l_y);
}

void EntityBase::setAcceleration(float l_x, float l_y) {
	m_acceleration = sf::Vector2f(l_x, l_y);
}

void EntityBase::applyFriction(float l_x, float l_y) {
	if (m_velocity.x != 0) {
		if (m_velocity.x < 0) {
			m_velocity.x += l_x;
		}
		else {
			m_velocity.x -= l_x;
		}
	}

	if (m_velocity.y != 0) {
		if (m_velocity.y < 0) {
			m_velocity.y += l_y;
		}
		else {
			m_velocity.y -= l_y;
		}
	}
}

void EntityBase::update(float l_dT) {
	Map* map = m_entityManager->getContext()->m_map;
	accelerate(0, map->getGravity());
	addVelocity(m_acceleration.x * l_dT, m_acceleration.y * l_dT);
	setAcceleration(0, 0);
	sf::Vector2f frictionValue;
	if (m_referenceTile) {
		frictionValue = m_referenceTile->m_friction;
	}
	else if (map->getDefaultTile()) {
		frictionValue = map->getDefaultTile()->m_friction;
	}
	else {
		frictionValue = m_friction;
	}

	float frictionX = (m_speed.x * frictionValue.x) * l_dT;
	float frictionY = (m_speed.y * frictionValue.y) * l_dT;
	applyFriction(frictionX, frictionY);

	sf::Vector2f deltaPos = m_velocity * l_dT;
	move(deltaPos.x, deltaPos.y);
	m_collidingOnX = false;
	m_collidingOnY = false;

	checkCollisions();
	resolveCollisions();
}

void EntityBase::updateAABB() {
	m_AABB.left = m_position.x;
	m_AABB.top  = m_position.y;
	m_AABB.width  = m_size.x;
	m_AABB.height = m_size.y;
}

void EntityBase::checkCollisions() {
	Map* map = m_entityManager->getContext()->m_map;
	unsigned int tileSize = map->getTileSize();
	int fromX = floor(m_AABB.left / tileSize);
	int toX   = floor((m_AABB.left + m_AABB.width) / tileSize);
	int fromY = floor(m_AABB.top / tileSize);
	int toY   = floor((m_AABB.top + m_AABB.height) / tileSize);

	for (int x = fromX; x <= toX; ++x) {
		for (int y = fromY; y <= toY; ++y) {
			Tile* tile = map->getTile(x, y);
			if (!tile) { continue; }
			sf::FloatRect rect(x * tileSize, y * tileSize, tileSize, tileSize);
			sf::FloatRect intersection;
			m_AABB.intersects(rect, intersection);
			float area = intersection.width * intersection.height;
			CollisionElement collision(area, tile->m_properties, rect);
			m_collisions.emplace_back(collision);
			if (tile->m_warp && m_type == EntityType::Player) { map->loadNext(); }
		}
	}
}

void EntityBase::resolveCollisions() {
	if (!m_collisions.empty()) {
		std::sort(m_collisions.begin(), m_collisions.end(), sortCollisions);
		Map* map = m_entityManager->getContext()->m_map;
		unsigned int tileSize = map->getTileSize();
		for (auto &itr : m_collisions) {
			if (!m_AABB.intersects(itr.m_tileBounds)) { continue; }
			float xDiff = (m_AABB.left + m_AABB.width / 2) - (itr.m_tileBounds.left + itr.m_tileBounds.width / 2);
			float yDiff = (m_AABB.top + m_AABB.height / 2) - (itr.m_tileBounds.top + itr.m_tileBounds.width / 2);
			float distance = 0;
			if (abs(xDiff) > abs(yDiff)) {
				if (xDiff > 0) {
					distance = (itr.m_tileBounds.left + itr.m_tileBounds.width) - m_AABB.left;
				}
				else {
					distance = -((m_AABB.left + m_AABB.width) - itr.m_tileBounds.left);
				}
				move(distance, 0);
				m_velocity.x = 0;
				m_collidingOnX = true;
			}
			else {
				if (yDiff > 0) {
					distance = (itr.m_tileBounds.top + itr.m_tileBounds.height) - m_AABB.top;
				}
				else {
					distance = -((m_AABB.top + m_AABB.height) - itr.m_tileBounds.top);
				}
				move(0, distance);
				m_velocity.y = 0;
				if (m_collidingOnY) { continue; }
				m_referenceTile = itr.m_tile;
				m_collidingOnY = true;
			}
		}
		m_collisions.clear();
	}
	if (!m_collidingOnY) { m_referenceTile = nullptr; }
}