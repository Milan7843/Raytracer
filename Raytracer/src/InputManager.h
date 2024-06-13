#pragma once

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

// ImGui
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include <stdio.h>
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <GLES2/gl2.h>
#endif

#include "imgui/imgui_stdlib.h"

// [Win32] Our example includes a copy of glfw3.lib pre-compiled with VS2010 to maximize ease of testing and compatibility with old VS compilers.
// To link with VS2010-era libraries, VS2015+ requires linking with legacy_stdio_definitions.lib, which we do using this pragma.
// Your own project should not be affected, as you are likely to link with a newer binary of GLFW that is adequate for your version of Visual Studio.
#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif

#include "imguizmo/ImGuizmo.h"

class Scene;

namespace InputManager
{
	enum class Modifier
	{
		ANY,
		NONE,
		CTRL,
		SHIFT,
		ALT,
		CTRL_SHIFT,
		CTRL_ALT,
		ALT_SHIFT,
		CTRL_ALT_SHIFT
	};

	enum class Action
	{
		NONE,
		TRANSFORM,
	};

	enum class TransformAction
	{
		TRANSLATE,
		ROTATE,
		SCALE
	};

	enum class Axis
	{
		NONE,
		X, Y, Z
	};

	enum class InputAction
	{
		MOVE_VIEW_TO_SELECTED,
		CANCEL_TRANSLATION,
		TRANSLATE,
		ROTATE,
		SCALE,
		AXIS_X,
		AXIS_Y,
		AXIS_Z,
		CLICK,
		MOVE_VIEW,
		MOVE_FORWARD,
		MOVE_BACKWARD,
		MOVE_LEFT,
		MOVE_RIGHT,
		MOVE_UP,
		MOVE_DOWN,
		SAVE,
		SAVE_AS,
		NEW_SCENE
	};

	enum class KeyType
	{
		MOUSE,
		KEYBOARD
	};

	struct Input
	{
		InputAction action;
		Modifier modifier;
		int key;
		KeyType type;
	};

	// Initialise the input manager with the window to take input on
	void initialise(GLFWwindow* newWindow);

	// Take input to set the action states
	void takeInput(Scene& scene);

	// Get the current action
	Action getCurrentAction();

	// Get the axis on which the current action is being performed
	Axis getCurrentAxis();

	// Reset the current action to have no action being performed
	void resetCurrentAction();

	ImGuizmo::OPERATION getCurrentActionAsImGuizmoOperation();

	// Check whether the key for a specific action is pressed.
	// Only returns true on the first frame the key is pressed.
	bool keyPressed(InputAction action);

	// Check whether the key for a specific action is currently pressed
	bool keyHeld(InputAction action);

	// Check whether the key for a specific action is not pressed.
	// Only returns true on the first frame the key is released.
	bool keyReleased(InputAction action);

	// Check whether the key for a specific action is currently not pressed
	bool keyUp(InputAction action);

	// Update all keybinds' previous values to have the new old values
	void updateKeyBindsPreviousValues();

	glm::vec2 getMousePosition();

	Modifier getModifierState();
};

