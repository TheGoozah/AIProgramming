//=== General Includes ===
#include "stdafx.h"
#include "Guard.h"
#include "../App_Steering/SteeringAgent.h"
#include "../App_Steering/SteeringBehaviors.h"
#include "Behaviors.h"

//=== Constructor & Destructor ===
Guard::~Guard()
{
	for (auto pb : m_vpBehaviorVec)
		SAFE_DELETE(pb);
	m_vpBehaviorVec.clear();

	SAFE_DELETE(m_pBehaviorTree);
	SAFE_DELETE(m_pAgent);
}

//=== Functions ===
void Guard::Initialize()
{
	//Create agent
	m_pAgent = new SteeringAgent();
	m_pAgent->SetMaxLinearSpeed(16.f);
	m_pAgent->SetAutoOrient(true);
	m_pAgent->SetMass(0.1f);
	m_pAgent->SetRenderBehaviour(true);
	//Target
	m_Target.Position = m_pAgent->GetPosition();
	//Create behaviors
	auto pSeekBehavior = new Seek();
	pSeekBehavior->SetTarget(&m_Target);
	m_vpBehaviorVec.push_back(pSeekBehavior);
	auto pWanderBehavior = new Wander();
	m_vpBehaviorVec.push_back(pWanderBehavior);
	//Set initial steering behavior
	m_pAgent->SetSteeringBehaviour(pWanderBehavior);

	//Create blackboard
	auto pBlackboard = new Elite::Blackboard;
	pBlackboard->AddData("Agent", m_pAgent);
	pBlackboard->AddData("Target", m_Target.Position);
	pBlackboard->AddData("TargetSet", false);
	pBlackboard->AddData("SeekBehavior", static_cast<ISteeringBehavior*>(pSeekBehavior));
	pBlackboard->AddData("WanderBehavior", static_cast<ISteeringBehavior*>(pWanderBehavior));
	pBlackboard->AddData("CloseToRadius", 1.0f);
	pBlackboard->AddData("IsBoxOpen", false);

	//Create behavior tree
	m_pBehaviorTree = new Elite::BehaviorTree(pBlackboard,
		new BehaviorSelector
		({
			new BehaviorSequence
			({
				new BehaviorConditional(HasTarget),
				new BehaviorConditional(NotCloseToTarget),
				new BehaviorAction(ChangeToSeek)
			}),
			new BehaviorSequence
			({
				new BehaviorSelector
				({
					new BehaviorSequence
					({
						new BehaviorConditional(HasTarget),
						new BehaviorConditional(IsBoxNotOpen),
						new BehaviorAction(OpenBox),
						new BehaviorAction(PickupItem)
					}),
					new BehaviorSequence
					({
						new BehaviorConditional(HasTarget),
						new BehaviorAction(PickupItem)
					})
				}),
				new BehaviorAction(ChangeToWander)
			})
		}));
}

void Guard::Update(float deltaTime, const Elite::Vector2& targetPos, bool isSet)
{
	//Change blackboard data
	auto pBB = m_pBehaviorTree->GetBlackboard();
	if (pBB != nullptr)
	{
		if (isSet)
		{
			m_Target.Position = targetPos;
			pBB->ChangeData("Target", m_Target.Position);
			pBB->ChangeData("TargetSet", isSet);

			//Determine with random value if box is open or not
			auto r = rand() % 2;
			auto isOpen = r == 0 ? true : false;
			pBB->ChangeData("IsBoxOpen", isOpen);
		}
	}
	//Update tree
	m_pBehaviorTree->Update();

	//Update agent
	m_pAgent->Update(deltaTime);
}

void Guard::Render(float deltaTime)
{
	//Render agent
	m_pAgent->Render(deltaTime);
}