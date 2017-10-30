#pragma once
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <vector>
#include <unordered_map>
#include <functional>
#include <fstream>
#include <sstream>
#include <iostream>

enum class EventType {
	KeyDown = sf::Event::KeyPressed,
	KeyUp = sf::Event::KeyReleased,
	MButtonDown = sf::Event::MouseButtonPressed,
	MButtonUp = sf::Event::MouseButtonReleased,
	MouseWheel = sf::Event::MouseWheelMoved,
	WindowResized = sf::Event::Resized,
	GainedFocus = sf::Event::GainedFocus,
	LostFocus = sf::Event::LostFocus,
	MouseEntered = sf::Event::MouseEntered,
	MouseLeft = sf::Event::MouseLeft,
	Closed = sf::Event::Closed,
	TextEntered = sf::Event::TextEntered,
	Keyboard = sf::Event::Count + 1, Mouse, Joystick
};

struct EventInfo {
	EventInfo() { m_code = 0; }
	EventInfo(int l_event) { m_code = l_event; }
	union {
		int m_code;
	};
};

struct EventDetails {
	EventDetails(const std::string& l_bindName)
		: m_name(l_bindName) {
		clear();
	}
	std::string m_name;

	sf::Vector2i m_size;
	sf::Uint32 m_textEntered;
	sf::Vector2i m_mouse;
	int m_mouseWheelDelta;
	int m_keyCode; // Single key code.

	void clear() {
		m_size = sf::Vector2i(0, 0);
		m_textEntered = 0;
		m_mouse = sf::Vector2i(0, 0);
		m_mouseWheelDelta = 0;
		m_keyCode = -1;
	}
};

using Events = std::vector<std::pair<EventType, EventInfo>>;

struct Binding {
	Binding(const std::string& l_name) : m_name(l_name), m_details(l_name), c(0) {}
	void bindEvent(EventType l_type, EventInfo l_info = EventInfo()) {
		m_events.emplace_back(l_type, l_info);
	}

	Events m_events;
	std::string m_name;
	int c; // Count of events that are "happening".

	EventDetails m_details;
};

using Bindings = std::unordered_map<std::string, Binding*>;
using CallbackContainer = std::unordered_map<std::string, std::function<void(EventDetails*)>>;
// State callback container.
enum class StateType;
using Callbacks = std::unordered_map<StateType, CallbackContainer>;


class EventManager {
public:
	EventManager();
	~EventManager();

	bool addBinding(Binding *l_binding);
	bool removeBinding(std::string l_name);

	void setFocus(const bool& l_focus);
	void setCurrentState(StateType l_state);

	// Needs to be defined in the header!
	template<class T>
	bool addCallback(StateType l_state, const std::string& l_name,
		void(T::*l_func)(EventDetails*), T* l_instance)
	{
		auto itr = m_callbacks.emplace(l_state, CallbackContainer()).first;
		auto temp = std::bind(l_func, l_instance, std::placeholders::_1);
		return itr->second.emplace(l_name, temp).second;
	}

	bool removeCallback(const StateType& l_state, const std::string& l_name);

	void handleEvent(sf::Event& l_event);
	void update();

	sf::Vector2i getMousePos(sf::RenderWindow* l_wind = nullptr) {
		return (l_wind ? sf::Mouse::getPosition(*l_wind) : sf::Mouse::getPosition());
	}
private:
	void loadBindings();

	StateType m_currentState;
	Bindings m_bindings;
	Callbacks m_callbacks;
	bool m_hasFocus;
};

/*#pragma once
#include <SFML\Graphics.hpp>
#include <unordered_map>
#include <functional>
#include <vector>

struct EventDetails {
EventDetails(const std::string& l_name) : m_name(l_name) { clear(); }

std::string m_name;
sf::Vector2i m_size;
sf::Uint32 m_textEntered;
sf::Vector2i m_mouse;
int m_mouseWheelDelta;
int m_keyCode;

void clear() {
m_size = sf::Vector2i(0, 0);
m_textEntered = 0;
m_mouse = sf::Vector2i(0, 0);
m_mouseWheelDelta = 0;
m_keyCode = -1;
}
};

struct Binding {
Binding(const std::string& l_name) : m_name(l_name), m_details(l_name), m_count(0) {}
void bindEvent(sf::Event::EventType& l_event, int l_code = 0) {
m_events.emplace_back(l_event, l_code);
}

std::vector<std::pair<sf::Event::EventType, int>> m_events;
EventDetails m_details;
std::string m_name;
int m_count;
};

using Bindings = std::unordered_map<std::string, Binding*>;
using Callbacks = std::unordered_map <std::string, std::function<void(EventDetails*)>>;


class EventManager {
public:
EventManager();
~EventManager();
bool addBinding(Binding* l_bind);
bool removeBinding(const std::string& l_bindName);

void handleEvent(sf::Event& l_event);
void update();
void setFocus(const bool& l_focus);
sf::Vector2i getMousePos(sf::RenderWindow* l_window = nullptr);

template <class T>
bool addCallback(const std::string& l_name, void (T::*l_func)(EventDetails*), T* l_instance) {
std::function<void(EventDetails*)> func = std::bind(l_func, l_instance, std::placeholders::_1);
return m_callbacks.emplace(l_name, func).second;
}

void removeCallback(const std::string& l_name) {
m_callbacks.erase(l_name);
}

private:
void loadBindings(const std::string& l_filePath);

Bindings m_bindings;
Callbacks m_callbacks;
bool m_isFocused;
}; */