#include "EntityManager.h"

EntityManager::EntityManager(SharedContext* l_context) : m_context(l_context) {
	loadEnemyTypes("Assets/Characters/EnemyList.list");
}

EntityManager::~EntityManager() {}

SharedContext* EntityManager::getContext() { return m_context; }


int EntityManager::add(const EntityType& l_type, const std::string& l_name = "") {

}

void EntityManager::remove(unsigned int l_id) {

}

void EntityManager::purge() {

}

void EntityManager::update(float l_time) {

}

void EntityManager::draw() {

}

EntityBase* EntityManager::find(const std::string& l_name) {
	for (auto &itr : m_entities) {
		if (itr.second->getName() == l_name) {
			return itr.second;
		}
	}
	return nullptr;
}

EntityBase* EntityManager::find(unsigned int l_id) {
	auto itr = m_entities.find(l_id);
	if (itr != m_entities.end()) {
		return itr->second;
	}
	return nullptr;
}

void EntityManager::processRemovals() {
	while (m_entitiesToRemove.begin != m_entitiesToRemove.end) {
		auto itr = m_entities.find(*m_entitiesToRemove.begin());
		if (itr != m_entities.end()) {
			delete itr->second;
			m_entities.erase(*m_entitiesToRemove.begin());
			m_entitiesToRemove.erase(m_entitiesToRemove.begin());
		}
	}
}

void EntityManager::loadEnemyTypes(const std::string& l_name) {

}

void EntityManager::entityCollisionCheck() {

}