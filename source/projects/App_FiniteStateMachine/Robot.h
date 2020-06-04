/*=============================================================================*/
// Copyright 2017-2018 Elite Engine
// Authors: Matthieu Delaere
/*=============================================================================*/
// Robot.h: Implementation of a Robot for Finite State Machine Demo
/*=============================================================================*/
#ifndef ELITE_APPLICATION_FSM_ROBOT
#define ELITE_APPLICATION_FSM_ROBOT

class Robot final
{
public:
	//--- Constructor & Destructor ---
	Robot() = default;
	~Robot();

	//--- Functions ---
	void Initialize();
	void Update(float deltaTime);

private:
	//--- Datamembers ---
	Elite::FiniteStateMachine* m_pStateMachine = nullptr;

	float m_UpdateTimer = 0.0f;
	float m_UpdateInterval = 0.25f;

	float m_BatteryCapacity = 40.0f;
	float m_BatteryPower = 25.0f;
	float m_CriticalBatteryLevel = 5.0f;
	float m_BatteryRechargeInterval = 6.0f;
	float m_BatteryDepletionInterval = 4.0f;

	bool m_FoundTrash = false;
	bool m_MovingTrash = false;

	//Containers
	std::vector<Elite::MulticastConditionBase*> m_Conditions = {};
	std::vector<Elite::MulticastBase*> m_Delegates = {};
	std::vector<Elite::FSMState*> m_States = {};

	//Functions
	static void Search();
	static void MoveToTrash(float& batteryPower, float depletionInterval);
	static void RemovingTrash(float& batteryPower, float depletionInterval);
	static void Refuel(float& batteryPower, float rechargeInterval);
};
#endif