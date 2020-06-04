//=== General Includes ===
#include "stdafx.h"
#include "Robot.h"
using namespace Elite;

//=== Constructor & Destructor ===
Robot::~Robot()
{
	//Cleanup
	SAFE_DELETE(m_pStateMachine);

	for (int i = 0; i < static_cast<int>(m_Conditions.size()); ++i)
		SAFE_DELETE(m_Conditions[i]);
	m_Conditions.clear();

	for (int i = 0; i < static_cast<int>(m_Delegates.size()); ++i)
		SAFE_DELETE(m_Delegates[i]);
	m_Delegates.clear();

	for (int i = 0; i < static_cast<int>(m_States.size()); ++i)
		SAFE_DELETE(m_States[i]);
	m_States.clear();
}

//=== Functions ===
void Robot::Initialize()
{
	//========================================================
	// This contains all the functionality for Demo purposes
	// This should be split in reusable files!
	//========================================================
	//Define condition lamba's
	auto lowBattery = [](float& batteryLevel, float batteryLimit)->bool
	{ return batteryLevel <= batteryLimit; };
	auto reachedRechargeLimit = [](float& batteryLevel, float batteryCapacity)->bool
	{return batteryLevel >= batteryCapacity; };
	auto hasFoundTrash = []()->bool
	{
		auto r = rand() % 6; //One of of 6 will find trash
		return r == 0 ? true : false;
	};
	auto hasReachedTrash = []()->bool
	{
		auto r = rand() % 6; //One of of 10 will reach trash
		return r == 0 ? true : false;
	};
	auto isMovingTrash = [&]()->bool
	{ return m_MovingTrash; };
	auto hasTrashInSight = [&]()->bool
	{ return m_FoundTrash; };


	//Define actions lambda's
	auto capBatteryLevel = [&]()->void
	{
		if (m_BatteryPower > m_BatteryCapacity)
			m_BatteryPower = m_BatteryCapacity;
	};

	auto setMovingTrash = [&](bool state)->void
	{m_MovingTrash = state; };

	auto setTrashInSight = [&](bool state)->void
	{m_FoundTrash = state; };

	//---------------------------------------------
	//Define transition conditions
	MulticastConditionBase* conditionLowBattery = new MulticastCondition<float&, float>(
	{ MulticastContainer<bool, float&, float>(lowBattery, m_BatteryPower, m_CriticalBatteryLevel) });
	m_Conditions.push_back(conditionLowBattery);

	MulticastConditionBase* conditionRecharged = new MulticastCondition<float&, float>(
	{ MulticastContainer<bool, float&, float>(reachedRechargeLimit, m_BatteryPower, m_BatteryCapacity) });
	m_Conditions.push_back(conditionRecharged);

	MulticastConditionBase* foundTrash = new MulticastCondition<>(
	{ MulticastContainer<bool>{ hasFoundTrash } });
	m_Conditions.push_back(foundTrash);

	MulticastConditionBase* reachedTrash = new MulticastCondition<>(
	{ MulticastContainer<bool>(hasReachedTrash) });
	m_Conditions.push_back(reachedTrash);

	MulticastConditionBase* movingTrash = new MulticastCondition<>(
	{ MulticastContainer<bool>(isMovingTrash) });
	m_Conditions.push_back(movingTrash);

	MulticastConditionBase* trashInSight = new MulticastCondition<>(
	{ MulticastContainer<bool>(hasTrashInSight) });
	m_Conditions.push_back(trashInSight);

	//---------------------------------------------
	//Define actions
	MulticastBase* refuelFunc = new Multicast<float&, float>(
	{ MulticastContainer<void, float&, float>(Refuel, this->m_BatteryPower, m_BatteryRechargeInterval) });
	m_Delegates.push_back(refuelFunc);

	MulticastBase* capBatteryLevelFunc = new Multicast<>(
	{ MulticastContainer<void>(capBatteryLevel) });
	m_Delegates.push_back(capBatteryLevelFunc);

	MulticastBase* searchFunc = new Multicast<>(
	{ MulticastContainer<void>(Search) });
	m_Delegates.push_back(searchFunc);

	MulticastBase* setNotRemovingFunc = new Multicast<bool>(
	{ MulticastContainer<void, bool>(setMovingTrash, false) });
	m_Delegates.push_back(setNotRemovingFunc);

	MulticastBase* setRemovingFunc = new Multicast<bool>(
	{ MulticastContainer<void, bool>(setMovingTrash, true) });
	m_Delegates.push_back(setRemovingFunc);

	MulticastBase* setFoundTrashFunc = new Multicast<bool>(
	{ MulticastContainer<void, bool>(setTrashInSight, true) });
	m_Delegates.push_back(setFoundTrashFunc);

	MulticastBase* setNotFoundTrashFunc = new Multicast<bool>(
	{ MulticastContainer<void, bool>(setTrashInSight, false) });
	m_Delegates.push_back(setNotFoundTrashFunc);

	MulticastBase* removeFunc = new Multicast<float&, float>(
	{ MulticastContainer<void, float&, float>(RemovingTrash, this->m_BatteryPower, m_BatteryDepletionInterval) });
	m_Delegates.push_back(removeFunc);

	MulticastBase* moveFunc = new Multicast<float&, float>(
	{ MulticastContainer<void, float&, float>(MoveToTrash, this->m_BatteryPower, m_BatteryDepletionInterval) });
	m_Delegates.push_back(moveFunc);

	//---------------------------------------------
	//Define states
	FSMState* refuelState = new FSMState();
	m_States.push_back(refuelState);
	FSMState* searchState = new FSMState();
	m_States.push_back(searchState);
	FSMState* moveState = new FSMState();
	m_States.push_back(moveState);
	FSMState* removeState = new FSMState();
	m_States.push_back(removeState);

	//---------------------------------------------
	//BUILD STATES by assigning actions, transitions and conditions
	//RefuelState
	refuelState->SetActions({ refuelFunc });
	refuelState->SetTransitions({
		FSMTransition({ conditionRecharged },{ capBatteryLevelFunc }, searchState) });

	//SearchState
	searchState->SetActions({ searchFunc });
	searchState->SetTransitions({
		FSMTransition({ movingTrash },{}, removeState),
		FSMTransition({ trashInSight },{}, moveState),
		FSMTransition({ foundTrash },{ setFoundTrashFunc }, moveState) });

	//MoveState
	moveState->SetActions({ moveFunc });
	moveState->SetTransitions({
		FSMTransition({ conditionLowBattery },{}, refuelState),
		FSMTransition({ reachedTrash },{ setRemovingFunc }, removeState) });

	//RemoveState
	removeState->SetActions({ removeFunc });
	removeState->SetTransitions({
		FSMTransition({ conditionLowBattery },{}, refuelState),
		FSMTransition({ foundTrash },{ setNotRemovingFunc, setNotFoundTrashFunc }, searchState) }); //Reuse our foundTrash for demo purposes :)

	//---------------------------------------------
	//Create state machine
	m_pStateMachine = new FiniteStateMachine({ refuelState, searchState, moveState, removeState }, searchState);
	//Boot FSM
	m_pStateMachine->Start();
}

void Robot::Update(float deltaTime)
{
	//Timer - Only update when > interval
	m_UpdateTimer += deltaTime;
	if (m_UpdateTimer >= m_UpdateInterval) //Update!
	{
		//Reset
		m_UpdateTimer = 0.0f;
		//Update FSM
		m_pStateMachine->Update();
	}
}

//=== Private Functions ===
void Robot::Search()
{
	std::cout << "Searching for garbage..." << std::endl;
}

void Robot::MoveToTrash(float& batteryPower, float depletionInterval)
{
	std::cout << "Moving to trash." << ", BatteryLevel: " << batteryPower << std::endl;
	batteryPower -= depletionInterval;
}

void Robot::RemovingTrash(float& batteryPower, float depletionInterval)
{
	std::cout << "Removing trash!" << ", BatteryLevel: " << batteryPower << std::endl;
	batteryPower -= depletionInterval;
}

void Robot::Refuel(float& batteryPower, float rechargeInterval)
{
	std::cout << "Refueling... " << ", BatteryLevel: " << batteryPower << std::endl;
	batteryPower += rechargeInterval;
}