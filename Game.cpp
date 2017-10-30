#include "Game.h"
#include <iostream>

Game::Game() : m_window("RPG", sf::Vector2u(800, 600)), m_stateManager(&m_shared)
{
	m_clock.restart();
	srand(time(nullptr));

	m_shared.m_wind = &m_window;
	m_shared.m_eventManager = m_window.getEventManager();
	m_shared.m_textureManager = &m_textureManager;
	m_stateManager.switchTo(StateType::Intro);
}

Game::~Game() {}

sf::Time Game::getElapsed() { return m_clock.getElapsedTime(); }
void Game::restartClock() { m_elapsed = m_clock.restart(); }
Window* Game::getWindow() { return &m_window; }

void Game::lateUpdate() {
	m_stateManager.processRequests();
	restartClock();
}

void Game::update() {
	m_window.update();
	m_stateManager.update(m_elapsed);
}

void Game::render() {
	m_window.beginDraw();
	m_stateManager.draw();
	m_window.endDraw();
}