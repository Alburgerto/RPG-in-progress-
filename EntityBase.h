#pragma once
#include <SFML\Graphics.hpp>

enum class EntityType { Base, Enemy, Player};
enum class EntityState { Idle, Walking, Jumping, Attacking, Hurt, Dying };

struct TileInfo;

struct CollisionElement {
	CollisionElement(float l_area, TileInfo* l_info, const sf::FloatRect& l_bounds)
		:m_area(l_area), m_tile(l_info), m_tileBounds(l_bounds) {}
	float m_area;
	TileInfo* m_tile;
	sf::FloatRect m_tileBounds;
};

using Collisions = std::vector<CollisionElement>;
bool SortCollisions(const CollisionElement& l_1, const CollisionElement& l_2);

class EntityManager;

class EntityBase {
friend class EntityManager;
public:
	EntityBase(EntityManager* l_entityManager);
	virtual ~EntityBase();

	const sf::Vector2f& getPosition()const;
	const sf::Vector2f& getSize()const;
	EntityState getState()const;
	std::string getName()const;
	unsigned int getId()const;
	EntityType getType()const;

	void setPosition(float l_x, float l_y);
	void setPosition(const sf::Vector2f& l_pos);
	void setSize(float l_x, float l_y);
	void setState(const EntityState& l_state);

	void move(float l_x, float l_y);
	void addVelocity(float l_x, float l_y);
	void accelerate(float l_x, float l_y);
	void setAcceleration(float l_x, float l_y);
	void applyFriction(float l_x, float l_y);

	virtual void update(float l_dT);
	virtual void draw(sf::RenderWindow* l_wind) = 0;

protected:

	void updateAABB();
	void checkCollisions();
	void resolveCollisions();

	// Method for what THIS entity does TO the l_collider entity.
	virtual void onEntityCollision(EntityBase* l_collider, bool l_attack) = 0;

	// Data members.
	std::string m_name;
	EntityType m_type;
	unsigned int m_id; // Entity id in the entity manager.
	sf::Vector2f m_position; // Current position.
	sf::Vector2f m_positionOld; // Position before entity moved.
	sf::Vector2f m_velocity; // Current velocity.
	sf::Vector2f m_maxVelocity; // Maximum velocity.
	sf::Vector2f m_speed; // Value of acceleration.
	sf::Vector2f m_acceleration; // Current acceleration.
	sf::Vector2f m_friction; // Default friction value.
	TileInfo* m_referenceTile; // Tile underneath entity.
	sf::Vector2f m_size; // Size of the collision box.
	sf::FloatRect m_AABB; // The bounding box for collisions.
	EntityState m_state; // Current entity state.
	
	// Flags for remembering axis collisions.
	bool m_collidingOnX;
	bool m_collidingOnY;

	Collisions m_collisions;
	EntityManager* m_entityManager;
};