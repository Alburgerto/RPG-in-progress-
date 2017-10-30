#pragma once
#include "BaseState.h"
#include "EventManager.h"

class State_Paused : public BaseState {
public:
	State_Paused(StateManager* l_stateManager);
	~State_Paused();

	void activate();
	void deactivate();

	void onCreate();
	void onDestroy();

	void update(const sf::Time& l_time);
	void draw();

	void unPause(EventDetails* l_details);

private:
	sf::RectangleShape m_rect;
	sf::Text m_text;
	sf::Font m_font;
};