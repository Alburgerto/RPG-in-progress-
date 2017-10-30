#include "State_Intro.h"
#include "StateManager.h"

State_Intro::State_Intro(StateManager* l_stateManager) : BaseState(l_stateManager) {}
State_Intro::~State_Intro() {}

// Not gonna de/activate after first launching game
void State_Intro::activate() {}
void State_Intro::deactivate() {}

void State_Intro::onCreate() {
	m_timePassed = 0.0f;
	m_transitionTime = 1.5f;

	m_texture.loadFromFile("Assets/Textures/intro.png");
	m_sprite.setTexture(m_texture);
	m_font.loadFromFile("Assets/Fonts/arial.ttf");
	m_text.setFont(m_font);
	m_text.setString(sf::String("Press SPACE to continue"));
	m_sprite.setOrigin(m_texture.getSize().x / 2, m_texture.getSize().y / 2);

	sf::FloatRect textRectangle = m_text.getLocalBounds();
	m_text.setOrigin(textRectangle.left + textRectangle.width / 2, textRectangle.top + textRectangle.height / 2);

	sf::Vector2u windowSize = m_stateManager->getContext()->m_wind->getSize();
	m_sprite.setPosition(windowSize.x / 2, 0);
	m_text.setPosition(m_sprite.getPosition().x, m_sprite.getPosition().y + m_texture.getSize().y * 2);

	EventManager* eventManager = m_stateManager->getContext()->m_eventManager;
	eventManager->addCallback(StateType::Intro, "Intro_Continue", &State_Intro::continueState, this);
}

void State_Intro::onDestroy() {
	m_stateManager->getContext()->m_eventManager->removeCallback(StateType::Intro, "Intro_Continue");
}

void State_Intro::update(const sf::Time& l_time) {

	if (m_timePassed < m_transitionTime) {
		m_timePassed += l_time.asSeconds();
		m_sprite.setPosition(m_sprite.getPosition().x, m_sprite.getPosition().y + 0.01);
		m_text.setPosition(m_sprite.getPosition().x, m_sprite.getPosition().y + m_sprite.getLocalBounds().height * 2);
	}
}

void State_Intro::draw() {
	Window* window = m_stateManager->getContext()->m_wind;
	window->draw(m_sprite);
	if (m_timePassed > m_transitionTime) {
		window->draw(m_text);
	}
}

void State_Intro::continueState(EventDetails* l_details) {
	if (m_timePassed >= m_transitionTime) {
		m_stateManager->switchTo(StateType::MainMenu);
		m_stateManager->remove(StateType::Intro);
	}
}