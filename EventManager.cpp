#include "EventManager.h"

EventManager::EventManager() : m_currentState(StateType(0)), m_hasFocus(true) { loadBindings(); }

EventManager::~EventManager() {
	for (auto &itr : m_bindings) {
		delete itr.second;
		itr.second = nullptr;
	}
}

void EventManager::setCurrentState(StateType l_state) {
	m_currentState = l_state;
}

bool EventManager::addBinding(Binding *l_binding) {
	if (m_bindings.find(l_binding->m_name) != m_bindings.end())
		return false;

	return m_bindings.emplace(l_binding->m_name, l_binding).second;
}

bool EventManager::removeBinding(std::string l_name) {
	auto itr = m_bindings.find(l_name);
	if (itr == m_bindings.end()) { return false; }
	delete itr->second;
	m_bindings.erase(itr);
	return true;
}

void EventManager::setFocus(const bool& l_focus) { m_hasFocus = l_focus; }

void EventManager::handleEvent(sf::Event& l_event) {
	// Handling SFML events.
	for (auto &b_itr : m_bindings) {
		Binding* bind = b_itr.second;
		for (auto &e_itr : bind->m_events) {
			EventType sfmlEvent = (EventType)l_event.type;
			if (e_itr.first != sfmlEvent) { continue; }
			if (sfmlEvent == EventType::KeyDown || sfmlEvent == EventType::KeyUp) {
				if (e_itr.second.m_code == l_event.key.code) {
					// Matching event/keystroke.
					// Increase count.
					if (bind->m_details.m_keyCode != -1) {
						bind->m_details.m_keyCode = e_itr.second.m_code;
					}
					++(bind->c);
					break;
				}
			}
			else if (sfmlEvent == EventType::MButtonDown || sfmlEvent == EventType::MButtonUp) {
				if (e_itr.second.m_code == l_event.mouseButton.button) {
					// Matching event/keystroke.
					// Increase count.
					bind->m_details.m_mouse.x = l_event.mouseButton.x;
					bind->m_details.m_mouse.y = l_event.mouseButton.y;
					if (bind->m_details.m_keyCode != -1) {
						bind->m_details.m_keyCode = e_itr.second.m_code;
					}
					++(bind->c);
					break;
				}
			}
			else {
				// No need for additional checking.
				if (sfmlEvent == EventType::MouseWheel) {
					bind->m_details.m_mouseWheelDelta = l_event.mouseWheel.delta;
				}
				else if (sfmlEvent == EventType::WindowResized) {
					bind->m_details.m_size.x = l_event.size.width;
					bind->m_details.m_size.y = l_event.size.height;
				}
				else if (sfmlEvent == EventType::TextEntered) {
					bind->m_details.m_textEntered = l_event.text.unicode;
				}
				++(bind->c);
			}
		}
	}
}

void EventManager::update() {
	if (!m_hasFocus) { return; }
	for (auto &b_itr : m_bindings) {
		Binding* bind = b_itr.second;
		for (auto &e_itr : bind->m_events) {
			if (e_itr.first == EventType::Keyboard && sf::Keyboard::isKeyPressed(sf::Keyboard::Key(e_itr.second.m_code))) {
				if (bind->m_details.m_keyCode != -1) {
					bind->m_details.m_keyCode = e_itr.second.m_code;
				}
				++(bind->c);
			}
			else if (e_itr.first == EventType::Mouse && sf::Mouse::isButtonPressed(sf::Mouse::Button(e_itr.second.m_code))) {
				if (bind->m_details.m_keyCode != -1) {
					bind->m_details.m_keyCode = e_itr.second.m_code;
				}
				++(bind->c);
			}
		}
		if (bind->m_events.size() == bind->c) {
			auto stateCallbacks = m_callbacks.find(m_currentState);
			auto otherCallbacks = m_callbacks.find(StateType(0));

			if (stateCallbacks != m_callbacks.end()) {
				auto callItr = stateCallbacks->second.find(bind->m_name);
				if (callItr != stateCallbacks->second.end()) {
					callItr->second(&bind->m_details);
				}
			}

			if (otherCallbacks != m_callbacks.end()) {
				auto callItr = otherCallbacks->second.find(bind->m_name);
				if (callItr != otherCallbacks->second.end()) {
					// Pass in information about events.
					callItr->second(&bind->m_details);
				}
			}
		}
		bind->c = 0;
		bind->m_details.clear();
	}
}

void EventManager::loadBindings() {
	std::string delimiter = ":";

	std::ifstream bindings;
	bindings.open("Assets/Bindings/keys.cfg.txt");
	if (!bindings.is_open()) { std::cout << "! Failed loading keys.cfg." << std::endl; return; }
	std::string line;
	while (std::getline(bindings, line)) {
		std::stringstream keystream(line);
		std::string callbackName;
		keystream >> callbackName;
		Binding* bind = new Binding(callbackName);
		while (!keystream.eof()) {
			std::string keyval;
			keystream >> keyval;
			int start = 0;
			int end = keyval.find(delimiter);
			if (end == std::string::npos) { delete bind; bind = nullptr; break; }
			EventType type = EventType(stoi(keyval.substr(start, end - start)));
			int code = stoi(keyval.substr(end + delimiter.length(),
				keyval.find(delimiter, end + delimiter.length())));
			EventInfo eventInfo;
			eventInfo.m_code = code;

			bind->bindEvent(type, eventInfo);
		}

		if (!addBinding(bind)) { delete bind; }
		bind = nullptr;
	}
	bindings.close();
}

bool EventManager::removeCallback(const StateType& l_state, const std::string& l_name) {
	auto itr = m_callbacks.find(l_state);
	if (itr == m_callbacks.end()) { return false; }
	auto itr2 = itr->second.find(l_name);
	if (itr2 == itr->second.end()) { return false; }
	itr->second.erase(l_name);
	return true;
}

/* #include "EventManager.h"
#include <fstream>
#include <sstream>
#include <string>
#include <iostream>

EventManager::EventManager() : m_isFocused(true) {
loadBindings("Assets/Bindings/keys.cfg.txt");
}

EventManager::~EventManager() {
for (auto& binding : m_bindings) {
delete binding.second;
binding.second = nullptr;
}
}


void EventManager::setFocus(const bool& l_focus) {
m_isFocused = l_focus;
}

sf::Vector2i EventManager::getMousePos(sf::RenderWindow* l_window) {
return (l_window ? sf::Mouse::getPosition(*l_window) : sf::Mouse::getPosition()) ;
}


void EventManager::loadBindings(const std::string& l_filePath) {
std::ifstream file(l_filePath);
if (!file.is_open()) {
std::cerr << "Error opening file: " << l_filePath << std::endl;
}
std::string buffer;
while (std::getline(file, buffer)) {
std::stringstream sstream(buffer);
std::string bindName;
sstream >> bindName;
Binding* bind = new Binding(bindName);

std::string keyVal;
while (!sstream.eof()) {
sstream >> keyVal;
int pos = keyVal.find(':');
if (pos == std::string::npos) {
delete bind;
bind = nullptr;
break;
}
std::string keyString = keyVal.substr(0, pos);
int key = std::stoi(keyVal.substr(0, pos));
int eventCode = std::stoi(keyVal.substr(pos + 1, keyVal.size() - keyString.size() - 1));

sf::Event::EventType event = sf::Event::EventType(key);
bind->bindEvent(event, eventCode);
}
if (!addBinding(bind)) {
delete bind;
}
}
file.close();
}

bool EventManager::addBinding(Binding* l_bind) {
if (m_bindings.find(l_bind->m_name) != m_bindings.end()) {
return false;
}
return m_bindings.emplace(l_bind->m_name, l_bind).second;
}

bool EventManager::removeBinding(const std::string& l_bindName) {
if (m_bindings.find(l_bindName) == m_bindings.end()) {
return false;
}
delete m_bindings.at(l_bindName);
m_bindings.erase(l_bindName);
return true;
}

void EventManager::handleEvent(sf::Event& l_event) {
for (auto &b_itr : m_bindings) {
Binding* bind = b_itr.second;
for (auto &e_itr : bind->m_events) {
if (e_itr.first != l_event.type) { continue; }
if (l_event.type == sf::Event::EventType::KeyPressed || l_event.type == sf::Event::EventType::KeyReleased) {
if (e_itr.second == l_event.key.code) {
// Matching event/keystroke.
// Increase count.
if (bind->m_details.m_keyCode != -1) {
bind->m_details.m_keyCode = e_itr.second;
}
++(bind->m_count);
break;
}
}
else if (l_event.type == sf::Event::EventType::MouseButtonPressed || l_event.type == sf::Event::EventType::MouseButtonReleased) {
if (e_itr.second == l_event.mouseButton.button) {
// Matching event/keystroke.
// Increase count.
bind->m_details.m_mouse.x = l_event.mouseButton.x;
bind->m_details.m_mouse.y = l_event.mouseButton.y;
if (bind->m_details.m_keyCode != -1) {
bind->m_details.m_keyCode = e_itr.second;
}
++(bind->m_count);
break;
}
}
else {
// No need for additional checking.
if (l_event.type == sf::Event::EventType::MouseWheelMoved) {
bind->m_details.m_mouseWheelDelta = l_event.mouseWheel.delta;
}
else if (l_event.type == sf::Event::EventType::Resized) {
bind->m_details.m_size.x = l_event.size.width;
bind->m_details.m_size.y = l_event.size.height;
}
else if (l_event.type == sf::Event::EventType::TextEntered) {
bind->m_details.m_textEntered = l_event.text.unicode;
}
++(bind->m_count);
}
}
}
}

void EventManager::update() {
if (!m_isFocused) { return; }
for (auto& binding : m_bindings) {
Binding* bind = binding.second;
for (auto& event : bind->m_events) {
if (//event.first == sf::Event::EventType::KeyPressed &&
sf::Keyboard::isKeyPressed(sf::Keyboard::Key(event.second))) {
if (bind->m_details.m_keyCode != -1) {
bind->m_details.m_keyCode = event.second;
}
++(bind->m_count);
break;
}
else if (event.first == sf::Event::EventType::MouseButtonPressed &&
sf::Mouse::isButtonPressed(sf::Mouse::Button(event.second))) {
if (bind->m_details.m_keyCode != -1) {
bind->m_details.m_keyCode = event.second;
}
++(bind->m_count);
break;
}
}
if (bind->m_count == bind->m_events.size()) {
//			m_callbacks.at(bind->m_name)(&bind->m_details);
auto callback = m_callbacks.find(bind->m_name);
if (callback != m_callbacks.end()) {
callback->second(&bind->m_details);
}
}

bind->m_count = 0;
bind->m_details.clear();
}
} */