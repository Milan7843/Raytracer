#include "InputManager.h"

#include "Scene.h"

namespace InputManager
{
	namespace
	{
		unsigned int translateKey{ GLFW_KEY_G };
		unsigned int rotateKey{ GLFW_KEY_R };
		unsigned int scaleKey{ GLFW_KEY_S };
		unsigned int translateKeyPreviousState{ GLFW_RELEASE };
		unsigned int rotateKeyPreviousState{ GLFW_RELEASE };
		unsigned int scaleKeyPreviousState{ GLFW_RELEASE };

		unsigned int axisKeyX{ GLFW_KEY_X };
		unsigned int axisKeyY{ GLFW_KEY_Y };
		unsigned int axisKeyZ{ GLFW_KEY_Z };
		unsigned int axisKeyXPreviousState{ GLFW_RELEASE };
		unsigned int axisKeyYPreviousState{ GLFW_RELEASE };
		unsigned int axisKeyZPreviousState{ GLFW_RELEASE };

		Action currentAction{ Action::NONE };
		TransformAction currentTransformAction{ TransformAction::TRANSLATE };
		Axis currentAxis{ Axis::NONE };

		GLFWwindow* window;

		bool isClicked(unsigned int key, unsigned int previousState)
		{
			return glfwGetKey(window, key) == GLFW_PRESS &&
				previousState == GLFW_RELEASE;
		}
	}

	void initialise(GLFWwindow* newWindow)
	{
		window = newWindow;
	}

	void takeInput(Scene& scene)
	{
		switch (currentAction)
		{
			case Action::NONE:
				if (glfwGetKey(window, translateKey) == GLFW_PRESS && scene.hasObjectSelected())
				{
					currentAction = Action::TRANSFORM;
					currentTransformAction = TransformAction::TRANSLATE;
				}
				else if (glfwGetKey(window, rotateKey) == GLFW_PRESS && scene.hasObjectSelected())
				{
					currentAction = Action::TRANSFORM;
					currentTransformAction = TransformAction::ROTATE;
				}
				else if (glfwGetKey(window, scaleKey) == GLFW_PRESS && scene.hasObjectSelected())
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
				if (glfwGetKey(window, translateKey) == GLFW_PRESS && scene.hasObjectSelected())
				{
					currentAction = Action::TRANSFORM;
					currentTransformAction = TransformAction::TRANSLATE;
				}
				else if (glfwGetKey(window, rotateKey) == GLFW_PRESS && scene.hasObjectSelected())
				{
					currentAction = Action::TRANSFORM;
					currentTransformAction = TransformAction::ROTATE;
				}
				else if (glfwGetKey(window, scaleKey) == GLFW_PRESS && scene.hasObjectSelected())
				{
					currentAction = Action::TRANSFORM;
					currentTransformAction = TransformAction::SCALE;
				}

				// Setting the axis based on the key press 
				if (isClicked(axisKeyX, axisKeyXPreviousState))
				{
					std::cout << "on axis X" << std::endl;
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
				if (isClicked(axisKeyY, axisKeyYPreviousState))
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
				if (isClicked(axisKeyZ, axisKeyZPreviousState))
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

		translateKeyPreviousState = glfwGetKey(window, translateKey);
		rotateKeyPreviousState = glfwGetKey(window, rotateKey);
		scaleKeyPreviousState = glfwGetKey(window, scaleKey);
		axisKeyXPreviousState = glfwGetKey(window, axisKeyX);
		axisKeyYPreviousState = glfwGetKey(window, axisKeyY);
		axisKeyZPreviousState = glfwGetKey(window, axisKeyZ);
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
						std::cout << "on axis X" << std::endl;
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
};
