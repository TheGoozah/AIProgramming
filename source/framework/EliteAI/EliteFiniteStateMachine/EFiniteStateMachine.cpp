//=== General Includes ===
#include "stdafx.h"
#include "EFiniteStateMachine.h"
using namespace Elite;

//=== Functions ===
void FiniteStateMachine::Start()
{}

void FiniteStateMachine::Update()
{
	//Check all transitions for current state and see if we need to transition!
	auto transitions = m_CurrentState->GetTransitions();
	auto transitionTriggered = false;
	FSMTransition triggeredTransition = {};
	for (auto trans : transitions)
	{
		transitionTriggered = trans.IsTriggered();
		if (transitionTriggered)
		{
			triggeredTransition = trans;
			break;
		}
	}

	//If a transition triggered, perform the switching
	if (transitionTriggered)
	{
		//Get target state
		auto targetState = triggeredTransition.GetTargetState();

		//End current state, do transition actions and enter the new state
		m_CurrentState->RunExitActions();
		triggeredTransition.RunActions();
		targetState->RunEntryActions();

		//Switch
		m_CurrentState = targetState;
	}
	else //Else continue performing the current actions
	{
		m_CurrentState->RunActions();
	}
}