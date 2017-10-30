#include "State_Paused.h"
#include "StateManager.h"

State_Paused::State_Paused(StateManager* l_stateManager) : BaseState(l_stateManager) {}
State_Paused::~State_Paused() {}

void State_Paused::onCreate() {
	m_transparent = true;
	m_font.loadFromFile("Assets/Fonts/arial.ttf");
	m_text.setFont(m_font);
	m_text.setString("PAUSED");
	m_text.setCharacterSize(20);
	m_text.setStyle(sf::Text::Bold);

	sf::FloatRect rect = m_text.getLocalBounds();
	m_text.setOrigin(rect.left + rect.width / 2, rect.top + rect.height / 2);

	Window* window = m_stateManager->getContext()->m_wind;

	sf::Vector2f centerWindow = sf::Vector2f(window->getSize().x / 2, window->getSize().y / 2);
	m_text.setPosition(centerWindow);

	m_rect.setSize((sf::Vector2f)window->getSize());
	m_rect.setPosition(0, 0);
	m_rect.setFillColor({ 0, 0, 0, 150 });

	m_stateManager->getContext()->m_eventManager->addCallback(StateType::Paused, "Key_P", &State_Paused::unPause, this);
	m_stateManager->getContext()->m_eventManager->addCallback(StateType::Paused, "Key_Escape", &State_Paused::unPause, this);
}

void State_Paused::onDestroy() {
	m_stateManager->getContext()->m_eventManager->removeCallback(StateType::Paused, "Key_P");
	m_stateManager->getContext()->m_eventManager->removeCallback(StateType::Paused, "Key_Escape");
}

void State_Paused::draw() {
	m_stateManager->getContext()->m_wind->draw(m_rect);
	m_stateManager->getContext()->m_wind->draw(m_text);
}

void State_Paused::unPause(EventDetails* l_details) {
	m_stateManager->switchTo(StateType::Game);
}

void State_Paused::update(const sf::Time& l_time) {}
void State_Paused::activate() {}
void State_Paused::deactivate() {}