#include "StateManager.h"

StateManager::StateManager(SharedContext* l_shared) : m_shared(l_shared) {
	registerState<State_Intro>(StateType::Intro);
	registerState<State_MainMenu>(StateType::MainMenu);
	registerState<State_Game>(StateType::Game);
	registerState<State_Paused>(StateType::Paused); 
}

StateManager::~StateManager() {
	for (auto& state : m_states) {
		state.second->onDestroy();
		delete state.second;
	}
}

void StateManager::update(const sf::Time& l_time) {
	if (m_states.empty()) { return; }
	if (m_states.size() > 1 && m_states.back().second->getTranscendent()) {
		int i;
		for (i = m_states.size() - 1; i >= 0; --i) {
			if (!m_states[i].second->getTranscendent()) {
				break;
			}
		}
		for (; i < m_states.size(); ++i) {
			m_states[i].second->update(l_time);
		}
	} 
	else {
		m_states.back().second->update(l_time);
	}
}

void StateManager::draw() {
	if (m_states.empty()) { return; }
	if (m_states.size() > 1 && m_states.back().second->getTransparent()) {
		int i = m_states.size() -1;
		for (; i >= 0; --i) {
			if (!m_states[i].second->getTransparent()) {
				break;
			}
		}
		for (; i < m_states.size(); ++i) {
			m_shared->m_wind->getRenderWindow()->setView(m_states[i].second->getView());
			m_states[i].second->draw();
		}
	}
	else {
		m_states.back().second->draw();
	}
}

SharedContext* StateManager::getContext() {
	return m_shared;
}

bool StateManager::hasState(const StateType& l_state) {
	for (auto itr = m_states.begin(); itr < m_states.end(); ++itr) {
		if (itr->first == l_state) {
			auto itr2 = std::find(m_toRemove.begin(), m_toRemove.end(), l_state);
			if (itr2 == m_toRemove.end()) {
				return true;
			}
			return false;
		}
	}
	return false;
}

void StateManager::switchTo(const StateType& l_state) {
	m_shared->m_eventManager->setCurrentState(l_state);
	for (auto itr = m_states.begin(); itr < m_states.end(); ++itr) {
		if (itr->first == l_state) {
			m_states.back().second->deactivate();
			StateType tmp_type = itr->first;
			BaseState* tmp_state = itr->second;
			m_shared->m_wind->getRenderWindow()->setView(tmp_state->getView());
			m_states.erase(itr);
			m_states.emplace_back(tmp_type, tmp_state);
			m_states.back().second->activate();
			return;
		}
	}
	// State not found
	if (!m_states.empty()) { m_states.back().second->deactivate(); }
	createState(l_state);
	m_states.back().second->activate();
	m_shared->m_wind->getRenderWindow()->setView(m_states.back().second->getView());
}

void StateManager::remove(const StateType& l_state) {
	m_toRemove.push_back(l_state);
}

void StateManager::processRequests() {
	while (m_toRemove.begin() != m_toRemove.end()) {
		removeState(*m_toRemove.begin());
		m_toRemove.erase(m_toRemove.begin());
	}
}

void StateManager::removeState(const StateType& l_state) {
	for (auto itr = m_states.begin(); itr < m_states.end(); ++itr) {
		if (itr->first == l_state) {
			itr->second->onDestroy();
			delete(itr->second);
			m_states.erase(itr);
			return;
		}
	}
}

void StateManager::createState(const StateType& l_state) {
	auto newState = m_stateFactory.find(l_state);
	if (newState == m_stateFactory.end()) { return; }
	BaseState* state = newState->second();
	state->m_view = m_shared->m_wind->getRenderWindow()->getDefaultView();
	m_states.emplace_back(l_state, state);
	state->onCreate();
}