/*=============================================================================*/
// Copyright 2017-2018 Elite Engine
// Authors: Matthieu Delaere
/*=============================================================================*/
// Guard.h: Implementation of a Guard for Behavior Tree Demo
/*=============================================================================*/
#ifndef ELITE_APPLICATION_BEHAVIOR_TREE_GUARD
#define ELITE_APPLICATION_BEHAVIOR_TREE_GUARD
//-----------------------------------------------------------------
// Includes & Forward Declarations
//-----------------------------------------------------------------
#include "../App_Steering/SteeringHelpers.h"
class SteeringAgent;
class ISteeringBehavior;

//Guard class
class Guard final
{
public:
	//--- Constructor & Destructor ---
	Guard() = default;
	~Guard();

	//--- Functions ---
	void Initialize();
	void Update(float deltaTime, const Elite::Vector2& targetPos, bool isSet);
	void Render(float deltaTime);

private:
	//--- Datamembers ---
	SteeringAgent* m_pAgent = nullptr;
	std::vector<ISteeringBehavior*> m_vpBehaviorVec = {};
	Elite::BehaviorTree* m_pBehaviorTree = nullptr;
	TargetData m_Target = {};
};
#endif