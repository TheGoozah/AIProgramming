/*=============================================================================*/
// Copyright 2017-2018 Elite Engine
// Authors: Matthieu Delaere
/*=============================================================================*/
// Behaviors.h: Implementation of certain reusable behaviors for Behavior Tree Demo
/*=============================================================================*/
#ifndef ELITE_APPLICATION_BEHAVIOR_TREE_BEHAVIORS
#define ELITE_APPLICATION_BEHAVIOR_TREE_BEHAVIORS
//-----------------------------------------------------------------
// Includes & Forward Declarations
//-----------------------------------------------------------------
#include "framework/EliteMath/EMath.h"
#include "framework/EliteAI/EliteBehaviorTree/EBehaviorTree.h"
#include "../App_Steering/SteeringAgent.h"
#include "../App_Steering/SteeringBehaviors.h"

//-----------------------------------------------------------------
// Behaviors
//-----------------------------------------------------------------
inline bool HasTarget(Elite::Blackboard* pBlackboard)
{
	//Get data
	auto hasTarget = false;
	auto dataAvailable = pBlackboard->GetData("TargetSet", hasTarget);
	if (!dataAvailable)
		return false;

	return hasTarget;
}

inline bool NotCloseToTarget(Elite::Blackboard* pBlackboard)
{
	//Get data
	SteeringAgent* pAgent = nullptr;
	auto targetPos = Elite::ZeroVector2;
	auto closeToRadius = 0.0f;
	auto dataAvailable =
		pBlackboard->GetData("Agent", pAgent)
		&& pBlackboard->GetData("Target", targetPos)
		&& pBlackboard->GetData("CloseToRadius", closeToRadius);
	if (!dataAvailable)
		return false;

	//Perform check
	if (!pAgent)
		return false;

	auto distance = (targetPos - pAgent->GetPosition()).Magnitude();
	if (distance > closeToRadius)
		return true;

	return false;
}

inline BehaviorState ChangeToSeek(Elite::Blackboard* pBlackboard)
{
	//Get data
	ISteeringBehavior* pSeekBehavior = nullptr;
	SteeringAgent* pAgent = nullptr;
	auto dataAvailable =
		pBlackboard->GetData("Agent", pAgent)
		&& pBlackboard->GetData("SeekBehavior", pSeekBehavior);
	if (!dataAvailable)
		return Failure;

	//Perform action
	if (!pAgent || !pSeekBehavior)
		return Failure;

	if (pAgent->GetSteeringBehaviour() != pSeekBehavior)
	{
		printf("Come over here \n");
		pAgent->SetSteeringBehaviour(pSeekBehavior);
	}

	return Success;
}

inline BehaviorState ChangeToWander(Elite::Blackboard* pBlackboard)
{
	//Get data
	ISteeringBehavior* pWanderBehavior = nullptr;
	SteeringAgent* pAgent = nullptr;
	auto dataAvailable =
		pBlackboard->GetData("Agent", pAgent)
		&& pBlackboard->GetData("WanderBehavior", pWanderBehavior);
	if (!dataAvailable)
		return Failure;

	//Perform action
	if (!pAgent || !pWanderBehavior)
		return Failure;

	if (pAgent->GetSteeringBehaviour() != pWanderBehavior)
		pAgent->SetSteeringBehaviour(pWanderBehavior);

	printf("Just minding my own business \n");

	return Success;
}

inline bool IsBoxNotOpen(Elite::Blackboard* pBlackboard)
{
	//Get data
	auto isBoxOpen = false;
	auto dataAvailable = pBlackboard->GetData("IsBoxOpen", isBoxOpen);
	if (!dataAvailable)
		return false;

	if (isBoxOpen)
		printf("	Hooray, the box is already open! \n");

	return !isBoxOpen;
}

inline BehaviorState OpenBox(Elite::Blackboard* pBlackboard)
{
	printf("	Opening Box! \n");
	return Success;
}

inline BehaviorState PickupItem(Elite::Blackboard* pBlackboard)
{
	//Reached target, reset TargetSet
	pBlackboard->ChangeData("TargetSet", false);

	printf("	Taking Item! \n");
	return Success;
}
#endif