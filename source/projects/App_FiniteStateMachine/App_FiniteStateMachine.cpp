//Precompiled Header [ALWAYS ON TOP IN CPP]
#include "stdafx.h"

//Includes
#include "App_FiniteStateMachine.h"
#include "Robot.h"
using namespace Elite;

//Constructor & Destructor
App_FiniteStateMachine::App_FiniteStateMachine() : IApp()
{}

App_FiniteStateMachine::~App_FiniteStateMachine()
{
	SAFE_DELETE(m_pRobot);
}

//Functions
void App_FiniteStateMachine::Start()
{
	m_pRobot = new Robot();
	m_pRobot->Initialize();
}

void App_FiniteStateMachine::Update(float deltaTime)
{
	//Update that is being called after the physics simulation
	m_pRobot->Update(deltaTime);
}

void App_FiniteStateMachine::Render(float deltaTime) const
{
	//Update that is being called after the physics simulation
	(void)deltaTime;
}