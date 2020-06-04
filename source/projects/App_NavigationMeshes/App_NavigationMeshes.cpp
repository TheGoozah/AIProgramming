//Precompiled Header [ALWAYS ON TOP IN CPP]
#include "stdafx.h"

//Includes
#include "App_NavigationMeshes.h"
#include "NavigationColliderElement.h"
#include "../App_Steering/SteeringAgent.h"
#include "../App_Steering/SteeringBehaviors.h"
using namespace Elite;

//Constructor & Destructor
App_NavigationMeshes::App_NavigationMeshes() : IApp()
{}

App_NavigationMeshes::~App_NavigationMeshes()
{
	for (auto pNC : m_vNavigationColliders)
		SAFE_DELETE(pNC);
	m_vNavigationColliders.clear();

	SAFE_DELETE(m_pNavigationMesh);
	SAFE_DELETE(m_pNavMeshPathfinder);
	SAFE_DELETE(m_pSeekBehaviour);
	SAFE_DELETE(m_pAgent);
}

//Functions
void App_NavigationMeshes::Start()
{
	//----------- CAMERA ------------
	DEBUGRENDERER2D->GetActiveCamera()->SetZoom(29.282f);
	DEBUGRENDERER2D->GetActiveCamera()->SetCenter(Elite::Vector2(0.9361f, 0.2661f));
	DEBUGRENDERER2D->GetActiveCamera()->SetMoveLocked(true);
	DEBUGRENDERER2D->GetActiveCamera()->SetZoomLocked(true);

	//----------- AGENT ------------
	m_pSeekBehaviour = new Seek();
	m_Target = TargetData(Elite::ZeroVector2);
	m_pAgent = new SteeringAgent();
	m_pAgent->SetSteeringBehaviour(m_pSeekBehaviour);
	m_pAgent->SetMaxLinearSpeed(16.f);
	m_pAgent->SetAutoOrient(true);
	m_pAgent->SetMass(0.1f);

	//----------- WORLD ------------
	m_vNavigationColliders.push_back(new NavigationColliderElement(Elite::Vector2(15.f, 10.f), 14.0f, 1.0f));
	m_vNavigationColliders.push_back(new NavigationColliderElement(Elite::Vector2(-25.f, 10.f), 14.0f, 1.0f));
	m_vNavigationColliders.push_back(new NavigationColliderElement(Elite::Vector2(-13.f, -8.f), 30.0f, 2.0f));
	m_vNavigationColliders.push_back(new NavigationColliderElement(Elite::Vector2(15.f, -21.f), 14.0f, 1.0f));

	//----------- NAVIGATION MESH ------------
	std::list<Elite::Vector2> baseBox
	{ { -60, 30 },{ -60, -30 },{ 60, -30 },{ 60, 30 } };
	NavigationMeshBuilder navigationMeshBuilder = NavigationMeshBuilder();
	m_pNavigationMesh = navigationMeshBuilder.GenerateNavMesh(Elite::Polygon(baseBox), m_AgentRadius, 100);
	m_pNavMeshPathfinder = new Elite::NavigationMeshPathfinder();
}

void App_NavigationMeshes::Update(float deltaTime)
{
	//Update that is being called after the physics simulation
	(void)deltaTime;

	//Update target based on input
	if (INPUTMANAGER->IsMouseButtonUp(InputMouseButton::eMiddle))
	{
		auto mouseData = INPUTMANAGER->GetMouseData(Elite::InputType::eMouseButton, Elite::InputMouseButton::eMiddle);
		m_PathTarget = DEBUGRENDERER2D->GetActiveCamera()->ConvertScreenToWorld(
			Elite::Vector2((float)mouseData.X, (float)mouseData.Y));
	}

	//Find the path using the pathfinder
	m_vPath = m_pNavMeshPathfinder->FindPath(m_pNavigationMesh,
		NavigationMeshNode(m_pAgent->GetPosition(), -1, nullptr),
		NavigationMeshNode(m_PathTarget, -1, nullptr),
		HeuristicFunctions::Chebyshev);

	//Update target
	if (m_vPath.size() > 0)
		m_Target.Position = m_vPath[0];
	m_pSeekBehaviour->SetTarget(&m_Target);

	//Stop cheat
	auto stopDistance = m_AgentRadius / 2.0f;
	if ((m_pAgent->GetPosition() - m_Target.Position).SqrtMagnitude() < (stopDistance * stopDistance))
		m_pAgent->SetPosition(m_Target.Position);

	//Actor
	m_pAgent->Update(deltaTime);
}

void App_NavigationMeshes::Render(float deltaTime) const
{
	//Update that is being called after the physics simulation
	(void)deltaTime;

	//Render the elements
	for (auto pNC : m_vNavigationColliders)
		pNC->RenderElement();

	//Render debug information pathfinder
	Elite::NavigationMeshPathfinder::sDrawDebugInfoQueries = true;
	Elite::NavigationMeshPathfinder::sDrawPortals = true;

	//Render navigation mesh
	if (m_ShowPolygon)
		DEBUGRENDERER2D->DrawSolidPolygon(m_pNavigationMesh->GetPolygon(), Color(.7f, .85f, .45f, 1.f), 0.9f);

	//DRAW PATH
	if (m_ShowPath)
	{
		if (m_vPath.size() > 0)
		{
			auto color = Color(1.0f, 1.0f, 0.0f, 1.0f);
			DEBUGRENDERER2D->DrawSegment(m_pAgent->GetPosition(), m_vPath[0], color);
			DEBUGRENDERER2D->DrawPoint(m_vPath[0], 7.0f, color, .2f);
			for (size_t i = 1; i < m_vPath.size(); ++i)
			{
				DEBUGRENDERER2D->DrawSegment(m_vPath[i - 1], m_vPath[i], color);
				DEBUGRENDERER2D->DrawPoint(m_vPath[i], 7.0f, color, .2f);
			}
			DEBUGRENDERER2D->DrawSegment(m_vPath[m_vPath.size() - 1], m_PathTarget, color);
			DEBUGRENDERER2D->DrawPoint(m_PathTarget, 7.0f, color, .2f);
		}
	}
}