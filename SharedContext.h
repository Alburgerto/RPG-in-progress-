#pragma once
#include "Window.h"
#include "EventManager.h"
#include "TextureManager.h"
#include "Map.h"

struct SharedContext {
	SharedContext()
		:m_wind(nullptr),
		m_eventManager(nullptr),
		m_textureManager(nullptr),
		m_map(nullptr) {}
	Window* m_wind;
	EventManager* m_eventManager;
	TextureManager* m_textureManager;
	Map* m_map;
};