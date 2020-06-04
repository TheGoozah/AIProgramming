//Precompiled Header [ALWAYS ON TOP IN CPP]
#include "stdafx.h"

//-----------------------------------------------------------------
// Includes
//-----------------------------------------------------------------
//Application
#include "EliteInterfaces/EIApp.h"
#include "projects/App_Selector.h"

//---------- Registered Applications -----------
#ifdef Navigation
	#include "projects/App_NavigationMeshes/App_NavigationMeshes.h"
#endif
#ifdef BehaviorTree
	#include "projects/App_BehaviorTree/App_BehaviorTree.h"
#endif
#ifdef FiniteStateMachine
	#include "projects/App_FiniteStateMachine/App_FiniteStateMachine.h"
#endif

//Main
#undef main //Undefine SDL_main as main
int main(int argc, char* argv[])
{
	//Unreference parameter
	(void)argc;
	(void)argv;

	try
	{
		//Window Creation
		Elite::WindowParams params;
		EliteWindow* pWindow = new EliteWindow();
		pWindow->CreateEWindow(params);

		//Create Frame (can later be extended by creating FrameManager for MultiThreaded Rendering)
		EliteFrame* pFrame = new EliteFrame();
		pFrame->CreateFrame(pWindow);

		//Create a 2D Camera for debug rendering in this case
		Camera2D* pCamera = new Camera2D(params.width, params.height);
		DEBUGRENDERER2D->Initialize(pCamera);

		//Create Physics
		PHYSICSWORLD; //Boot

		//Start Timer
		TIMER->Start();

		//Application Creation
		IApp* myApp = nullptr;

#ifdef Navigation
		myApp = new App_NavigationMeshes();
#endif
#ifdef BehaviorTree
		myApp = new App_BehaviorTree();
#endif
#ifdef FiniteStateMachine
		myApp = new App_FiniteStateMachine();
#endif

		//Boot application
		myApp->Start();

		//Application Loop
		while (!pWindow->ShutdownRequested())
		{
			//Timer
			TIMER->Update();
			auto elapsed = TIMER->GetElapsed();

			//Window procedure first, to capture all events and input received by the window
			pWindow->ProcedureEWindow();

			//Update (Physics, App)
			PHYSICSWORLD->Simulate();
			pCamera->Update();
			myApp->Update(elapsed);

			//Render and Present Frame
			PHYSICSWORLD->RenderDebug();
			myApp->Render(elapsed);
			pFrame->SubmitAndFlipFrame();
		}

		//Reversed Deletion
		SAFE_DELETE(myApp);
		SAFE_DELETE(pCamera);
		SAFE_DELETE(pFrame);
		SAFE_DELETE(pWindow);

		//Shutdown All Singletons
		PHYSICSWORLD->Destroy();
		DEBUGRENDERER2D->Destroy();
		INPUTMANAGER->Destroy();
		TIMER->Destroy();
	}
	catch (const Elite_Exception& e)
	{
		std::cout << e._msg << " Error: " << SDL_GetError() << std::endl;
		return 1;
	}

	return 0;
}