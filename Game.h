#pragma once
#include "Window.h"
#include "EventManager.h"
#include "StateManager.h"
#include "SharedContext.h"
#include "TextureManager.h"

class Game {
public:
	Game();
	~Game();

	void handleInput();
	void update();
	void render();

	sf::Time getElapsed();
	Window* getWindow();

	void lateUpdate();

private:
	void restartClock();

	Window m_window;
	StateManager m_stateManager;
	TextureManager m_textureManager;
	SharedContext m_shared;

	sf::Clock m_clock;
	sf::Time m_elapsed;
};