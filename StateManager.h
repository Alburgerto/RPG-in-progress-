#pragma once
#include <vector>
#include <unordered_map>
#include "State_Intro.h"
#include "State_MainMenu.h"
#include "State_Game.h"
#include "State_Paused.h"
#include "SharedContext.h"

enum class StateType { Intro = 1, MainMenu, Game, Paused, GameOver, Credits };

using StateContainer = std::vector<std::pair<StateType, BaseState*>>;
using TypeContainer = std::vector<StateType>;
using StateFactory = std::unordered_map<StateType, std::function<BaseState*(void)>>;

class StateManager {
public:
	StateManager(SharedContext* l_shared);
	~StateManager();

	void update(const sf::Time& l_time);
	void draw();

	SharedContext* getContext();
	bool hasState(const StateType& l_state);

	void switchTo(const StateType& l_state);
	void remove(const StateType& l_state);

	void processRequests();
private:

	void removeState(const StateType& l_state);
	void createState(const StateType& l_state);

	template<class T>
	void registerState(const StateType& l_state) {
		m_stateFactory[l_state] = [this]() -> BaseState* 
		{
			return new T(this); // returns pointer to newly allocated memory
		};						// Passing "this" (StateManager pointer) to BaseState constructor
	}

	SharedContext* m_shared;
	StateContainer m_states;
	TypeContainer m_toRemove;
	StateFactory m_stateFactory;
};