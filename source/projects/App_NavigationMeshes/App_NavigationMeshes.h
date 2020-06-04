#ifndef NAVIGATION_MESHES_APPLICATION_H
#define NAVIGATION_MESHES_APPLICATION_H
//-----------------------------------------------------------------
// Includes & Forward Declarations
//-----------------------------------------------------------------
#include "framework/EliteInterfaces/EIApp.h"
#include "../App_Steering/SteeringHelpers.h"
class NavigationColliderElement;
class NavigationMesh;
class NavigationMeshPathfinder;
class SteeringAgent;
class Seek;

//-----------------------------------------------------------------
// Application
//-----------------------------------------------------------------
class App_NavigationMeshes final : public IApp
{
public:
	//Constructor & Destructor
	App_NavigationMeshes();
	virtual ~App_NavigationMeshes();

	//App Functions
	void Start() override;
	void Update(float deltaTime) override;
	void Render(float deltaTime) const override;

private:
	//Datamembers
	//--Agent--
	SteeringAgent* m_pAgent = nullptr;
	Seek* m_pSeekBehaviour = nullptr;
	TargetData m_Target = {};
	float m_AgentRadius = 1.0f;
	Elite::Vector2 m_PathTarget = Elite::ZeroVector2;

	//--Level--
	std::vector<NavigationColliderElement*> m_vNavigationColliders = {};

	//--NavMesh--
	Elite::NavigationMesh* m_pNavigationMesh = nullptr;

	//--Pathfinder--
	Elite::NavigationMeshPathfinder* m_pNavMeshPathfinder = nullptr;
	std::vector<Elite::Vector2> m_vPath;

	//-- Helper Rendering (for debugging purposes) ---
	bool m_ShowPolygon = true;
	bool m_ShowPath = true;

	//C++ make the class non-copyable
	App_NavigationMeshes(const App_NavigationMeshes&) {};
	App_NavigationMeshes& operator=(const App_NavigationMeshes&) {};
};
#endif