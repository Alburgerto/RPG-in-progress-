#pragma once
#include "BaseState.h"
#include "EventManager.h"

class State_Game : public BaseState {
public:
	State_Game(StateManager* l_stateManager);
	~State_Game();

	void activate();
	void deactivate();

	void onCreate();
	void onDestroy();

	void update(const sf::Time& l_time);
	void draw();

	void pause(EventDetails* l_details);
	void menu(EventDetails* l_details);

private:
	sf::Sprite m_sprite;
	sf::Texture m_texture;
	sf::Vector2f m_velocity;
	sf::Vector2f m_windowSize;
};