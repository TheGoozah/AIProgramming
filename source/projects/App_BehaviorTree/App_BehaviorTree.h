#ifndef BEHAVIOR_TREE_APPLICATION_H
#define BEHAVIOR_TREE_APPLICATION_H
//-----------------------------------------------------------------
// Includes & Forward Declarations
//-----------------------------------------------------------------
#include "framework/EliteInterfaces/EIApp.h"
#include "../App_Steering/SteeringHelpers.h"
class Guard;

//-----------------------------------------------------------------
// Application
//-----------------------------------------------------------------
class App_BehaviorTree final : public IApp
{
public:
	//Constructor & Destructor
	App_BehaviorTree();
	virtual ~App_BehaviorTree();

	//App Functions
	void Start() override;
	void Update(float deltaTime) override;
	void Render(float deltaTime) const override;

private:
	//Datamembers
	Guard* m_pGuard = nullptr;
	Goal m_Goal = {};

	//C++ make the class non-copyable
	App_BehaviorTree(const App_BehaviorTree&) {};
	App_BehaviorTree& operator=(const App_BehaviorTree&) {};
};
#endif