#include "InputManager.h"

#include "Scene.h"

namespace InputManager
{
	namespace
	{
		const unsigned int numKeyBinds{ 19 };

		Input inputs[numKeyBinds] = {
			{
				InputAction::MOVE_VIEW_TO_SELECTED,
				Modifier::ANY,
				GLFW_KEY_Q,
				KeyType::KEYBOARD
			},
			{
				InputAction::CANCEL_TRANSLATION,
				Modifier::ANY,
				GLFW_MOUSE_BUTTON_RIGHT,
				KeyType::MOUSE
			},
			{
				InputAction::TRANSLATE,
				Modifier::ANY,
				GLFW_KEY_G,
				KeyType::KEYBOARD
			},
			{
				InputAction::ROTATE,
				Modifier::ANY,
				GLFW_KEY_R,
				KeyType::KEYBOARD
			},
			{
				InputAction::SCALE,
				Modifier::ANY,
				GLFW_KEY_S,
				KeyType::KEYBOARD
			},
			{
				InputAction::AXIS_X,
				Modifier::ANY,
				GLFW_KEY_X,
				KeyType::KEYBOARD
			},
			{
				InputAction::AXIS_Y,
				Modifier::ANY,
				GLFW_KEY_Y,
				KeyType::KEYBOARD
			},
			{
				InputAction::AXIS_Z,
				Modifier::ANY,
				GLFW_KEY_Z,
				KeyType::KEYBOARD
			},
			{
				InputAction::CLICK,
				Modifier::ANY,
				GLFW_MOUSE_BUTTON_LEFT,
				KeyType::MOUSE
			},
			{
				InputAction::MOVE_VIEW,
				Modifier::ANY,
				GLFW_MOUSE_BUTTON_RIGHT,
				KeyType::MOUSE
			},
			{
				InputAction::MOVE_FORWARD,
				Modifier::ANY,
				GLFW_KEY_W,
				KeyType::KEYBOARD
			},
			{
				InputAction::MOVE_BACKWARD,
				Modifier::ANY,
				GLFW_KEY_S,
				KeyType::KEYBOARD
			},
			{
				InputAction::MOVE_LEFT,
				Modifier::ANY,
				GLFW_KEY_A,
				KeyType::KEYBOARD
			},
			{
				InputAction::MOVE_RIGHT,
				Modifier::ANY,
				GLFW_KEY_D,
				KeyType::KEYBOARD
			},
			{
				InputAction::MOVE_UP,
				Modifier::ANY,
				GLFW_KEY_E,
				KeyType::KEYBOARD
			},
			{
				InputAction::MOVE_DOWN,
				Modifier::ANY,
				GLFW_KEY_Q,
				KeyType::KEYBOARD
			},
			{
				InputAction::SAVE,
				Modifier::CTRL,
				GLFW_KEY_S,
				KeyType::KEYBOARD
			},
			{
				InputAction::SAVE_AS,
				Modifier::CTRL_SHIFT,
				GLFW_KEY_S,
				KeyType::KEYBOARD
			},
			{
				InputAction::NEW_SCENE,
				Modifier::CTRL,
				GLFW_KEY_N,
				KeyType::KEYBOARD
			}
		};

		int keyBindsPreviousState[numKeyBinds];

		Action currentAction{ Action::NONE };
		TransformAction currentTransformAction{ TransformAction::TRANSLATE };
		Axis currentAxis{ Axis::NONE };

		GLFWwindow* window;

		bool isClicked(unsigned int key, unsigned int previousState)
		{
			return glfwGetKey(window, key) == GLFW_PRESS &&
				previousState == GLFW_RELEASE;
		}

		int getKeyStateFromIndex(unsigned int keyIndex)
		{
			KeyType keyType = inputs[keyIndex].type;
			unsigned int key = inputs[keyIndex].key;

			switch (keyType)
			{
				case KeyType::KEYBOARD:
					return glfwGetKey(window, key);
				case KeyType::MOUSE:
					return glfwGetMouseButton(window, key);
			}
		}

		int getKeyState(InputAction action)
		{
			unsigned int actionIndex{ (unsigned int)action };
			return getKeyStateFromIndex(actionIndex);
		}
	}

	void initialise(GLFWwindow* newWindow)
	{
		window = newWindow;

		// Setting all key binds' initial states to be non-pressed
		for (unsigned int i{ 0 }; i < numKeyBinds; i++)
		{
			keyBindsPreviousState[i] = GLFW_RELEASE;
		}
	}

	void takeInput(Scene& scene)
	{
		switch (currentAction)
		{
			case Action::NONE:
				if (keyPressed(InputAction::TRANSLATE) && scene.hasObjectSelected())
				{
					currentAction = Action::TRANSFORM;
					currentTransformAction = TransformAction::TRANSLATE;
				}
				else if (keyPressed(InputAction::ROTATE) && scene.hasObjectSelected())
				{
					currentAction = Action::TRANSFORM;
					currentTransformAction = TransformAction::ROTATE;
				}
				else if (keyPressed(InputAction::SCALE) && scene.hasObjectSelected())
				{
					currentAction = Action::TRANSFORM;
					currentTransformAction = TransformAction::SCALE;
				}
				break;

			case Action::TRANSFORM:
				// If no object is selected, we go back to no action
				if (!scene.hasObjectSelected())
				{
					resetCurrentAction();
					break;
				}
				// Setting the transformation action based on the key press
				if (keyPressed(InputAction::TRANSLATE) && scene.hasObjectSelected())
				{
					currentAction = Action::TRANSFORM;
					currentTransformAction = TransformAction::TRANSLATE;
				}
				else if (keyPressed(InputAction::ROTATE) && scene.hasObjectSelected())
				{
					currentAction = Action::TRANSFORM;
					currentTransformAction = TransformAction::ROTATE;
				}
				else if (keyPressed(InputAction::SCALE) && scene.hasObjectSelected())
				{
					currentAction = Action::TRANSFORM;
					currentTransformAction = TransformAction::SCALE;
				}

				// Setting the axis based on the key press 
				if (keyPressed(InputAction::AXIS_X))
				{
					// If we were already on this axis, now go to no specific axis
					if (currentAxis == Axis::X)
					{
						currentAxis = Axis::NONE;
					}
					// Otherwise go to this axis
					else
					{
						currentAxis = Axis::X;
					}
				}
				if (keyPressed(InputAction::AXIS_Y))
				{
					// If we were already on this axis, now go to no specific axis
					if (currentAxis == Axis::Y)
					{
						currentAxis = Axis::NONE;
					}
					// Otherwise go to this axis
					else
					{
						currentAxis = Axis::Y;
					}
				}
				if (keyPressed(InputAction::AXIS_Z))
				{
					// If we were already on this axis, now go to no specific axis
					if (currentAxis == Axis::Z)
					{
						currentAxis = Axis::NONE;
					}
					// Otherwise go to this axis
					else
					{
						currentAxis = Axis::Z;
					}
				}
				break;
		}
	}

	Action getCurrentAction()
	{
		return currentAction;
	}

	Axis getCurrentAxis()
	{
		return currentAxis;
	}

	void resetCurrentAction()
	{
		currentAction = Action::NONE;
		currentAxis = Axis::NONE;
		currentTransformAction = TransformAction::TRANSLATE;
	}

	ImGuizmo::OPERATION getCurrentActionAsImGuizmoOperation()
	{
		switch (currentTransformAction)
		{
			case InputManager::TransformAction::TRANSLATE:
				switch (currentAxis)
				{
					case InputManager::Axis::NONE:
						return ImGuizmo::OPERATION::TRANSLATE;
					case InputManager::Axis::X:
						return ImGuizmo::OPERATION::TRANSLATE_X;
					case InputManager::Axis::Y:
						return ImGuizmo::OPERATION::TRANSLATE_Y;
					case InputManager::Axis::Z:
						return ImGuizmo::OPERATION::TRANSLATE_Z;
					default:
						return ImGuizmo::OPERATION::TRANSLATE;
				}
			case InputManager::TransformAction::ROTATE:
				switch (currentAxis)
				{
					case InputManager::Axis::NONE:
						return ImGuizmo::OPERATION::ROTATE;
					case InputManager::Axis::X:
						return ImGuizmo::OPERATION::ROTATE_X;
					case InputManager::Axis::Y:
						return ImGuizmo::OPERATION::ROTATE_Y;
					case InputManager::Axis::Z:
						return ImGuizmo::OPERATION::ROTATE_Z;
					default:
						return ImGuizmo::OPERATION::ROTATE;
				}
			case InputManager::TransformAction::SCALE:
				switch (currentAxis)
				{
					case InputManager::Axis::NONE:
						return ImGuizmo::OPERATION::SCALE;
					case InputManager::Axis::X:
						return ImGuizmo::OPERATION::SCALE_X;
					case InputManager::Axis::Y:
						return ImGuizmo::OPERATION::SCALE_Y;
					case InputManager::Axis::Z:
						return ImGuizmo::OPERATION::SCALE_Z;
					default:
						return ImGuizmo::OPERATION::SCALE;
				}
			default:
				return ImGuizmo::OPERATION::TRANSLATE;
		}
	}

	bool keyPressed(InputAction action)
	{
		unsigned int keyIndex{ (unsigned int)action };
		int currentState{ getKeyState(action) };
		int previousState{ keyBindsPreviousState[keyIndex] };

		bool rightModifier = inputs[keyIndex].modifier == Modifier::ANY || getModifierState() == inputs[keyIndex].modifier;

		// Must be pressed now and released on the previous frame
		return currentState == GLFW_PRESS && previousState == GLFW_RELEASE && rightModifier;
	}

	bool keyHeld(InputAction action)
	{
		unsigned int keyIndex{ (unsigned int)action };
		bool rightModifier = inputs[keyIndex].modifier == Modifier::ANY || getModifierState() == inputs[keyIndex].modifier;

		// Checking whether the key is currently pressed
		return getKeyState(action) == GLFW_PRESS && rightModifier;
	}

	bool keyReleased(InputAction action)
	{
		unsigned int keyIndex{ (unsigned int)action };
		int currentState{ getKeyState(action) };
		int previousState{ keyBindsPreviousState[keyIndex] };

		bool rightModifier = inputs[keyIndex].modifier == Modifier::ANY || getModifierState() == inputs[keyIndex].modifier;

		// Must be released now and pressed on the previous frame
		return currentState == GLFW_RELEASE && previousState == GLFW_PRESS;
	}

	bool keyUp(InputAction action)
	{
		unsigned int keyIndex{ (unsigned int)action };
		bool rightModifier = getModifierState() == inputs[keyIndex].modifier;

		// Checking whether the key is currently not pressed
		return getKeyState(action) == GLFW_RELEASE || !rightModifier;
	}

	void updateKeyBindsPreviousValues()
	{
		// Setting all key binds' states to the current values
		for (unsigned int i{ 0 }; i < numKeyBinds; i++)
		{
			keyBindsPreviousState[i] = getKeyStateFromIndex(i);
		}
	}

	glm::vec2 getMousePosition()
	{
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);

		return glm::vec2(xpos, ypos);
	}

	Modifier getModifierState()
	{
		bool ctrl = glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS ||
			glfwGetKey(window, GLFW_KEY_RIGHT_CONTROL) == GLFW_PRESS;
		bool alt = glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS ||
			glfwGetKey(window, GLFW_KEY_RIGHT_ALT) == GLFW_PRESS;
		bool shift = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS ||
			glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS;

		if (ctrl && !alt && !shift)
			return Modifier::CTRL;
		if (!ctrl && !alt && shift)
			return Modifier::SHIFT;
		if (!ctrl && alt && !shift)
			return Modifier::ALT;

		if (ctrl && !alt && shift)
			return Modifier::CTRL_SHIFT;
		if (ctrl && alt && !shift)
			return Modifier::CTRL_ALT;
		if (!ctrl && alt && shift)
			return Modifier::ALT_SHIFT;
		if (ctrl && alt && shift)
			return Modifier::CTRL_ALT_SHIFT;

		return Modifier::NONE;
	}
};
