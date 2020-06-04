#ifndef FINITE_STATE_MACHINE_APPLICATION_H
#define FINITE_STATE_MACHINE_APPLICATION_H
//-----------------------------------------------------------------
// Includes & Forward Declarations
//-----------------------------------------------------------------
#include "framework/EliteInterfaces/EIApp.h"
class Robot;

//-----------------------------------------------------------------
// Application
//-----------------------------------------------------------------
class App_FiniteStateMachine final : public IApp
{
public:
	//Constructor & Destructor
	App_FiniteStateMachine();
	virtual ~App_FiniteStateMachine();

	//App Functions
	void Start() override;
	void Update(float deltaTime) override;
	void Render(float deltaTime) const override;

private:
	//Datamembers
	Robot* m_pRobot = nullptr;

	//C++ make the class non-copyable
	App_FiniteStateMachine(const App_FiniteStateMachine&) {};
	App_FiniteStateMachine& operator=(const App_FiniteStateMachine&) {};
};
#endif