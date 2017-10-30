#include "State_Game.h"
#include "StateManager.h"

State_Game::State_Game(StateManager* l_stateManager) : BaseState(l_stateManager) {}
State_Game::~State_Game() {}

// Not gonna de/activate after first launching game
void State_Game::activate() {}
void State_Game::deactivate() {}

void State_Game::onCreate() {
	m_texture.loadFromFile("Assets/Textures/Mushroom.png");
	m_sprite.setTexture(m_texture);
	m_sprite.setOrigin(m_texture.getSize().x / 2, m_texture.getSize().y / 2);

	m_windowSize = sf::Vector2f(m_stateManager->getContext()->m_wind->getSize());
	sf::Vector2f centerWindow = sf::Vector2f(m_windowSize.x / 2, m_windowSize.y / 2);
	m_sprite.setPosition(centerWindow);
	m_velocity = sf::Vector2f(400, 400);

	EventManager* eventManager = m_stateManager->getContext()->m_eventManager;
	eventManager->addCallback(StateType::Game, "Key_P", &State_Game::pause, this);
	eventManager->addCallback(StateType::Game, "Key_Escape", &State_Game::menu, this);
}

void State_Game::onDestroy() {
	m_stateManager->getContext()->m_eventManager->removeCallback(StateType::Game, "Key_P");
	m_stateManager->getContext()->m_eventManager->removeCallback(StateType::Game, "Key_Escape");
}

void State_Game::update(const sf::Time& l_time) {
	m_sprite.setPosition(m_sprite.getPosition().x + (m_velocity.x * l_time.asSeconds()), 
						 m_sprite.getPosition().y + (m_velocity.y * l_time.asSeconds()));
	
	if (m_sprite.getPosition().x - m_texture.getSize().x / 2 < 0 ||
		m_sprite.getPosition().x + m_texture.getSize().x / 2 > m_windowSize.x) {
		m_velocity.x = -m_velocity.x;
	}
	else if (m_sprite.getPosition().y - m_texture.getSize().y / 2 < 0 ||
			 m_sprite.getPosition().y + m_texture.getSize().y / 2 >= m_windowSize.y) {
		m_velocity.y = -m_velocity.y;
	}
}

void State_Game::draw() {
	Window* window = m_stateManager->getContext()->m_wind;
	window->draw(m_sprite);
}

void State_Game::pause(EventDetails* l_details) {
	m_stateManager->switchTo(StateType::Paused);
}

void State_Game::menu(EventDetails* l_details) {
	m_stateManager->switchTo(StateType::MainMenu);
}
