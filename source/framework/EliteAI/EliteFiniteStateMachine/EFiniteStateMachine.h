/*=============================================================================*/
// Copyright 2017-2018 Elite Engine
// Authors: Matthieu Delaere
/*=============================================================================*/
// EFiniteStateMachine.h: Implementation of a FSM
/*=============================================================================*/
#ifndef ELITE_FINITE_STATE_MACHINE
#define ELITE_FINITE_STATE_MACHINE

//--- Includes ---
//#include "../../EliteHelpers/EMulticastDelegate.h"

namespace Elite
{
	//-----------------------------------------------------------------
	//TYPEDEF'S
	//-----------------------------------------------------------------
	typedef Elite::MulticastBase* FSMDelegateBasePtr;
	typedef Elite::MulticastConditionBase* FSMConditionBasePtr;

	//-----------------------------------------------------------------
	// STATE CLASS
	//-----------------------------------------------------------------
	class FSMTransition;
	class FSMState final
	{
	public:
		//--- Constructor & Destructor ---
		FSMState(std::vector<FSMDelegateBasePtr> entryActions, std::vector<FSMDelegateBasePtr> actions,
			std::vector<FSMDelegateBasePtr> exitActions, std::vector<FSMTransition> transitions) :
			m_EntryActions(entryActions), m_Actions(actions), m_ExitActions(exitActions), m_Transitions(transitions) {}
		FSMState() = default;
		virtual ~FSMState()
		{
			m_EntryActions.clear();
			m_Actions.clear();
			m_ExitActions.clear();
			m_Transitions.clear();
		}

		//--- Run Actions ---
		void RunActions() const { for (auto a : m_Actions) a->Invoke(); }; //The actions that should run when active
		void RunEntryActions() const { for (auto a : m_EntryActions) a->Invoke(); }; //The action that should run when entering this state
		void RunExitActions() const { for (auto a : m_ExitActions) a->Invoke(); }; //The action that should run when exiting this state

		//--- Setters ---
		void SetEntryActions(std::vector<FSMDelegateBasePtr> entryActions)
		{ m_EntryActions = entryActions; }
		void SetActions(std::vector<FSMDelegateBasePtr> actions)
		{ m_Actions = actions; }
		void SetExitActions(std::vector<FSMDelegateBasePtr> exitActions)
		{ m_ExitActions = exitActions; }
		void SetTransitions(std::vector<FSMTransition> transitions)
		{ m_Transitions = transitions; }

		//--- Getters ---
		std::vector<FSMTransition> GetTransitions() { return m_Transitions; };

	private:
		//--- Datamembers ---
		std::vector<FSMDelegateBasePtr> m_EntryActions = {};
		std::vector<FSMDelegateBasePtr> m_Actions = {};
		std::vector<FSMDelegateBasePtr> m_ExitActions = {};
		std::vector<FSMTransition> m_Transitions = {};
	};

	//-----------------------------------------------------------------
	// TRANSITION CLASS
	//-----------------------------------------------------------------
	class FSMTransition final
	{
	public:
		//--- Constructor & Destructor ---
		FSMTransition(std::vector<FSMConditionBasePtr> conditions, std::vector<FSMDelegateBasePtr> actions, FSMState* targetState) :
			m_TargetState(targetState), m_Conditions(conditions), m_Actions(actions) {}
		FSMTransition()
		{
			m_Conditions = {};
			m_Actions = {};
		}

		//--- Functions ---
		bool IsTriggered() const //Triggered when one of the given conditions is true
		{
			auto r = false;
			if (m_Conditions.size() > 0)
			{
				for (auto c : m_Conditions)
					r |= c->Invoke();
			}
			else
				r = true;
			return r;
		};
		void RunActions() const { for (auto a : m_Actions) a->Invoke(); };

		//--- Getters ---
		FSMState* GetTargetState() const { return m_TargetState; };

		//--- Setters ---
		void SetTargetState(FSMState* state)
		{ m_TargetState = state; }
		void SetConditions(std::vector<FSMConditionBasePtr> conditions)
		{ m_Conditions = conditions; }
		void SetActions(std::vector<FSMDelegateBasePtr> actions)
		{ m_Actions = actions; }

	private:
		//--- Datamembers ---
		FSMState* m_TargetState = {};
		std::vector<FSMConditionBasePtr> m_Conditions = {};
		std::vector<FSMDelegateBasePtr> m_Actions = {};
	};

	//-----------------------------------------------------------------
	// FINITE STATE MACHINE CLASS
	//-----------------------------------------------------------------
	class FiniteStateMachine final
	{
	public:
		//--- Constructor & Destructor ---
		FiniteStateMachine(std::vector<FSMState*> states, FSMState* initialState) :
			m_States(states), m_InitialState(initialState), m_CurrentState(initialState) {}
		virtual ~FiniteStateMachine()
		{ m_States.clear(); }

		//--- Functions ---
		void Start();
		void Update();

	private:
		//--- Datamembers ---
		std::vector<FSMState*> m_States = {};
		FSMState* m_InitialState = {};
		FSMState* m_CurrentState = {};
	};
}
#endif