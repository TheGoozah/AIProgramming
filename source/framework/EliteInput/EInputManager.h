/*=============================================================================*/
// Copyright 2017-2018 Elite Engine
// Authors: Matthieu Delaere
/*=============================================================================*/
// EInputManager.h: manager class that controls the input in the engine.
/*=============================================================================*/
#ifndef ELITE_INPUT_MANAGER
#define	ELITE_INPUT_MANAGER

namespace Elite
{
	//=== Forward Declaration ===
#if (PLATFORM_ID == PLATFORM_WINDOWS)
	class SDLWindow;
#endif

	//=== Data Structures ===
	struct KeyboardData //8 bytes
	{
		int TimeStamp = 0;
		InputScancode ScanCode = eScancode_Unknown;

		KeyboardData() = default;
		KeyboardData(int timeStamp, InputScancode keyCode) :
			TimeStamp(timeStamp), ScanCode(keyCode)
		{}
	};

	struct MouseData //== 24 bytes
	{
		int TimeStamp = 0;
		InputMouseButton Button = InputMouseButton(0);
		int X = 0; //Position X relative to window OR amound of scroll, based on Type!
		int Y = 0; //Position Y relative to window OR amound of scroll, based on Type!
		int XRel = 0;
		int YRel = 0; //Y == Direction when scrolling (1 == UP, -1 == DOWN)

		MouseData() = default;
		MouseData(int timeStamp, InputMouseButton button, int x, int y, 
			int xRel = 0, int yRel = 0):
			TimeStamp(timeStamp), Button(button), X(x), Y(y), 
			XRel(xRel), YRel(yRel)
		{}
	};

	union InputData //"Wasting" 16 bytes for a more user-friendly setup, SDL wastes even more memory (= 48 bytes)
	{
		MouseData MouseInputData;
		KeyboardData KeyboardInputData;

		InputData(MouseData data) : MouseInputData(data) {}
		InputData(KeyboardData data) : KeyboardInputData(data) {}
	};

	//=== Actual InputAction used by the InputManager ===
	struct InputAction
	{
		InputType InputActionType = InputType::eDefault;
		InputState InputActionState = eDown;
		InputData InputActionData;

		InputAction(InputType type, InputState state, InputData data) :
			InputActionType(type), InputActionState(state), InputActionData(data) {}
		InputAction() = default;
	};

	/*! EInputManager: manager class that controls all the input, captured from active platform & window*/
	class EInputManager final : public ESingleton<EInputManager>
	{
	public:
		bool IsKeyboardKeyDown(InputScancode key) { return IsKeyPresent(eKeyboard, eDown, key); };
		bool IsKeyboardKeyUp(InputScancode key) { return IsKeyPresent(eKeyboard, eReleased, key); }

		bool IsMouseButtonDown(InputMouseButton button) { return IsMousePresent(eMouseButton, eDown, button); }
		bool IsMouseButtonUp(InputMouseButton button) { return IsMousePresent(eMouseButton, eReleased, button); }
		bool IsMouseScrolling() { return IsMousePresent(eMouseWheel); }
		bool IsMouseMoving() { return IsMousePresent(eMouseMotion); }
		MouseData GetMouseData(InputType type, InputMouseButton button = InputMouseButton(0));

	private:
		//=== Friends ===
		//Our window has access to add input events to our queue, our application can later use these events
#if (PLATFORM_ID == PLATFORM_WINDOWS)
		friend SDLWindow;
#endif

		//=== Internal Functions
		void Flush(){ m_InputContainer.clear();};
		void AddInputAction(const InputAction& inputAction) 
		{ m_InputContainer.push_back(inputAction); };

		bool IsKeyPresent(InputType type, InputState state, InputScancode code);
		bool IsMousePresent(InputType type, InputState state = InputState(0), InputMouseButton button = InputMouseButton(0));

		//=== Datamembers ===
		std::vector<InputAction> m_InputContainer;
	};
}
#endif