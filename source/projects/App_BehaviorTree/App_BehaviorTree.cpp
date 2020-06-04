//Precompiled Header [ALWAYS ON TOP IN CPP]
#include "stdafx.h"

//Includes
#include "App_BehaviorTree.h"
#include "Guard.h"
using namespace Elite;

//Constructor & Destructor
App_BehaviorTree::App_BehaviorTree() : IApp()
{}

App_BehaviorTree::~App_BehaviorTree()
{
	SAFE_DELETE(m_pGuard);
}

//Functions
void App_BehaviorTree::Start()
{
	//Initialization of your application. If you want access to the physics world you will need to store it yourself.
	m_pGuard = new Guard();
	m_pGuard->Initialize();
}

void App_BehaviorTree::Update(float deltaTime)
{
	//Update that is being called after the physics simulation
	(void)deltaTime;

	//Capture input
	if (INPUTMANAGER->IsMouseButtonUp(InputMouseButton::eLeft))
	{
		auto mouseData = INPUTMANAGER->GetMouseData(Elite::InputType::eMouseButton, Elite::InputMouseButton::eLeft);
		m_Goal.Position = DEBUGRENDERER2D->GetActiveCamera()->ConvertScreenToWorld(
			Elite::Vector2((float)mouseData.X, (float)mouseData.Y));
		m_Goal.PositionSet = true;
	}

	//Update our guard
	m_pGuard->Update(deltaTime, m_Goal.Position, m_Goal.PositionSet);
	//Reset flag
	m_Goal.PositionSet = false;
}

void App_BehaviorTree::Render(float deltaTime) const
{
	//Update that is being called after the physics simulation
	(void)deltaTime;
	//Render our guard
	m_pGuard->Render(deltaTime);
}