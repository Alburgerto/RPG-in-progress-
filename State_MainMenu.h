#pragma once
#include "BaseState.h"
#include "EventManager.h"

class State_MainMenu : public BaseState {
public:
	State_MainMenu(StateManager* l_stateManager);
	~State_MainMenu();

	void activate();
	void deactivate();

	void onCreate();
	void onDestroy();

	void update(const sf::Time& l_time);
	void draw();

	void clickButton(EventDetails* l_details);

private:
	sf::Vector2f m_buttonSize;
	sf::Text m_text;
	sf::Text m_labels[2];
	sf::RectangleShape m_textRect[2];
	sf::Font m_font;
};