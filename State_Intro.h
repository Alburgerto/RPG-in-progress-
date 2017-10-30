#pragma once
#include "BaseState.h"
#include "EventManager.h"

class State_Intro : public BaseState {
public:
	State_Intro(StateManager* l_stateManager);
	~State_Intro();

	void activate();
	void deactivate();

	void onCreate();
	void onDestroy();

	void update(const sf::Time& l_time);
	void draw();

	void continueState(EventDetails* l_details);

private:
	sf::Sprite m_sprite;
	sf::Texture m_texture;
	sf::Text m_text;
	sf::Font m_font;
	float m_timePassed;
	float m_transitionTime;
};