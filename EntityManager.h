#pragma once
#include "SharedContext.h"
#include "EntityBase.h"

using EntityContainer = std::unordered_map<unsigned int, EntityBase*>;
using EntityFactory = std::unordered_map<EntityType, std::function<EntityBase*(void)>>;
using EnemyTypes = std::unordered_map < std::string, std::string>;

class EntityManager {
public:
	EntityManager(SharedContext* l_context);
	~EntityManager();

	int add(const EntityType& l_type, const std::string& l_name = "");
	void remove(unsigned int l_id);
	void purge();

	void update(float l_time);
	void draw();

	EntityBase* find(const std::string& l_name);
	EntityBase* find(unsigned int l_id);

	SharedContext* getContext();

private:

	template<typename T>
	void registerEntity(const EntityType& l_type) {
		m_entityFactory[l_type] = [this]() -> EntityBase* {
			return new T(this);
		};
	}

	void processRemovals();
	void loadEnemyTypes(const std::string& l_name);
	void entityCollisionCheck();

	SharedContext* m_context;

	EntityContainer m_entities;
	EntityFactory m_entityFactory;
	EnemyTypes m_enemyTypes;

	unsigned int m_maxEntities;
	unsigned int m_idCounter;

	std::vector<unsigned int> m_entitiesToRemove;
};