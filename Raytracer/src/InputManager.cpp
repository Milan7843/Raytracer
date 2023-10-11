#include "InputManager.h"

#include "Scene.h"

namespace InputManager
{
	namespace
	{
		const unsigned int numKeyBinds{ 9 };

		int keyBinds[] = {
			GLFW_KEY_Q,
			GLFW_MOUSE_BUTTON_RIGHT,
			GLFW_KEY_G,
			GLFW_KEY_R,
			GLFW_KEY_S,
			GLFW_KEY_X,
			GLFW_KEY_Y,
			GLFW_KEY_Z,
			GLFW_MOUSE_BUTTON_LEFT
		};

		KeyType keyTypes[]{
			KeyType::KEYBOARD,
			KeyType::MOUSE,
			KeyType::KEYBOARD,
			KeyType::KEYBOARD,
			KeyType::KEYBOARD,
			KeyType::KEYBOARD,
			KeyType::KEYBOARD,
			KeyType::KEYBOARD,
			KeyType::MOUSE
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
			KeyType keyType = keyTypes[keyIndex];
			unsigned int key = keyBinds[keyIndex];

			switch (keyType)
			{
				case KeyType::KEYBOARD:
					return glfwGetKey(window, key);
				case KeyType::MOUSE:
					return glfwGetMouseButton(window, key);
			}
		}

		int getKeyState(InputKey key)
		{
			unsigned int keyIndex{ (unsigned int)key };
			return getKeyStateFromIndex(keyIndex);
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
				if (keyPressed(InputKey::TRANSLATE) && scene.hasObjectSelected())
				{
					currentAction = Action::TRANSFORM;
					currentTransformAction = TransformAction::TRANSLATE;
				}
				else if (keyPressed(InputKey::ROTATE) && scene.hasObjectSelected())
				{
					currentAction = Action::TRANSFORM;
					currentTransformAction = TransformAction::ROTATE;
				}
				else if (keyPressed(InputKey::SCALE) && scene.hasObjectSelected())
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
				if (keyPressed(InputKey::TRANSLATE) && scene.hasObjectSelected())
				{
					currentAction = Action::TRANSFORM;
					currentTransformAction = TransformAction::TRANSLATE;
				}
				else if (keyPressed(InputKey::ROTATE) && scene.hasObjectSelected())
				{
					currentAction = Action::TRANSFORM;
					currentTransformAction = TransformAction::ROTATE;
				}
				else if (keyPressed(InputKey::SCALE) && scene.hasObjectSelected())
				{
					currentAction = Action::TRANSFORM;
					currentTransformAction = TransformAction::SCALE;
				}

				// Setting the axis based on the key press 
				if (keyPressed(InputKey::AXIS_X))
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
				if (keyPressed(InputKey::AXIS_Y))
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
				if (keyPressed(InputKey::AXIS_Z))
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

	bool keyPressed(InputKey key)
	{
		unsigned int keyIndex{ (unsigned int)key };
		int currentState{ getKeyState(key) };
		int previousState{ keyBindsPreviousState[keyIndex] };

		// Must be pressed now and released on the previous frame
		return currentState == GLFW_PRESS && previousState == GLFW_RELEASE;
	}

	bool keyHeld(InputKey key)
	{
		// Checking whether the key is currently pressed
		return getKeyState(key) == GLFW_PRESS;
	}

	bool keyReleased(InputKey key)
	{
		unsigned int keyIndex{ (unsigned int)key };
		int currentState{ getKeyState(key) };
		int previousState{ keyBindsPreviousState[keyIndex] };

		// Must be released now and pressed on the previous frame
		return currentState == GLFW_RELEASE && previousState == GLFW_PRESS;
	}

	bool keyUp(InputKey key)
	{
		// Checking whether the key is currently not pressed
		return getKeyState(key) == GLFW_RELEASE;
	}

	void updateKeyBindsPreviousValues()
	{
		// Setting all key binds' states to the current values
		for (unsigned int i{ 0 }; i < numKeyBinds; i++)
		{
			keyBindsPreviousState[i] = getKeyStateFromIndex(i);
		}
	}
};
