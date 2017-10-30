#pragma once
#include <SFML/Graphics.hpp>

class StateManager;

class BaseState {
	friend class StateManager;
public:
	BaseState(StateManager* l_stateManager) : m_stateManager(l_stateManager), m_transparent(false), m_transcendent(false) {}
	virtual ~BaseState() {}

	virtual void onCreate() = 0;
	virtual void onDestroy() = 0;

	virtual void activate() = 0;
	virtual void deactivate() = 0;

	virtual void update(const sf::Time& l_time) = 0;
	virtual void draw() = 0;

	bool getTransparent() const { return m_transparent; }
	void setTransparent(const bool& l_transparent) { m_transparent = l_transparent; }

	bool getTranscendent() const { return m_transcendent; }
	void setTranscendent(const bool& l_transcendent) { m_transcendent = l_transcendent; }

	StateManager* getStateManager() { return m_stateManager; }
	sf::View& getView() { return m_view; }
protected:
	StateManager* m_stateManager;
	sf::View m_view;
	bool m_transparent;
	bool m_transcendent;
};