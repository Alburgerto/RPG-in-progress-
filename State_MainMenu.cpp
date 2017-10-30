#include "State_MainMenu.h"
#include "StateManager.h"
#include <iostream>

State_MainMenu::State_MainMenu(StateManager* l_stateManager) : BaseState(l_stateManager) {}
State_MainMenu::~State_MainMenu() {}

void State_MainMenu::activate() {
	if (m_stateManager->hasState(StateType::MainMenu) && m_labels[0].getString() == "PLAY") {
		m_labels[0].setString("RESUME");
		sf::FloatRect rect = m_labels[0].getLocalBounds();
		m_labels[0].setOrigin(rect.left + rect.width / 2, rect.top + rect.height / 2);
	}
}


void State_MainMenu::onCreate() {
	m_font.loadFromFile("Assets/Fonts/arial.ttf");
	SharedContext* context = m_stateManager->getContext();
	m_buttonSize = sf::Vector2f(300, 30);

	m_text.setFont(m_font);
	m_text.setCharacterSize(30);
	m_text.setString("MAIN MENU");

	sf::FloatRect textSize = m_text.getLocalBounds();
	sf::FloatRect textRectSize;

	m_text.setOrigin(textSize.left + textSize.width / 2, textSize.top + textSize.height / 2);
	m_text.setPosition(context->m_wind->getSize().x / 2, 50);

	m_labels[0].setString("PLAY");
	m_labels[1].setString("EXIT");

	for (int i = 0; i < 2; ++i) {
		m_labels[i].setFont(m_font);
		m_labels[i].setCharacterSize(16);
		textSize = m_labels[i].getLocalBounds();
		m_labels[i].setOrigin(textSize.left + textSize.width / 2, textSize.top + textSize.height / 2);
		m_labels[i].setPosition(context->m_wind->getSize().x / 2, 250 + i * 70);

		m_textRect[i].setSize(m_buttonSize);
		textRectSize = m_textRect[i].getLocalBounds();
		m_textRect[i].setOrigin(textRectSize.left + textRectSize.width / 2, textRectSize.top + textRectSize.height / 2);
		m_textRect[i].setPosition(m_labels[i].getPosition());
		m_textRect[i].setFillColor(sf::Color(200, 200, 200, 100));
	}
	context->m_eventManager->addCallback(StateType::MainMenu, "Mouse_Left", &State_MainMenu::clickButton, this);
}

void State_MainMenu::onDestroy() {
	m_stateManager->getContext()->m_eventManager->removeCallback(StateType::MainMenu, "Mouse_Left");
}


void State_MainMenu::draw() {
	Window* window = m_stateManager->getContext()->m_wind;
	for (int i = 0; i < 2; ++i) {
		window->draw(m_textRect[i]);
		window->draw(m_labels[i]);
	}
	window->draw(m_text);
}

void State_MainMenu::clickButton(EventDetails* l_details) {
	SharedContext* context = m_stateManager->getContext();
	sf::Vector2i mousePos = context->m_eventManager->getMousePos(context->m_wind->getRenderWindow());
//	sf::Vector2i mousePos = l_details->m_mouse;

	if (m_textRect[0].getGlobalBounds().contains(mousePos.x, mousePos.y)) {
		m_stateManager->switchTo(StateType::Game);
	}
	else if (m_textRect[1].getGlobalBounds().contains(mousePos.x, mousePos.y)) {
		m_stateManager->getContext()->m_wind->close();
	}  
}

void State_MainMenu::update(const sf::Time& l_time) {}

void State_MainMenu::deactivate() {}