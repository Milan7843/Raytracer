#pragma once

#include <GLFW/glfw3.h>

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

	enum class InputKey
	{
		MOVE_VIEW_TO_SELECTED,
		CANCEL_TRANSLATION
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
	bool keyPressed(InputKey key);

	// Check whether the key for a specific action is currently pressed
	bool keyHeld(InputKey key);

	// Check whether the key for a specific action is not pressed.
	// Only returns true on the first frame the key is released.
	bool keyReleased(InputKey key);

	// Check whether the key for a specific action is currently not pressed
	bool keyUp(InputKey key);

	// Update all keybinds' previous values to have the new old values
	void updateKeyBindsPreviousValues();
};

