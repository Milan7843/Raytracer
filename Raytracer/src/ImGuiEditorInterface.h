#pragma once

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

#include "Logger.h"

class Scene;

enum ObjectType
{
	NONE,
	MESH,
	MODEL,
	SPHERE,
	POINT_LIGHT,
	DIRECTIONAL_LIGHT,
	AMBIENT_LIGHT,
	MATERIAL
};

class ImGuiEditorInterface
{
public:
	virtual bool drawInterface(Scene& scene) = 0;

	unsigned int getID()
	{
		return id;
	}

	ObjectType getType()
	{
		return this->type;
	}

protected:
	ImGuiEditorInterface()
	{
		// The instance count must start at 1, since 0 indicates nothing
		if (instanceCount == 0)
		{
			instanceCount++;
		}

		id = instanceCount++; // Assign the current instance count and increment it
	}
	virtual ~ImGuiEditorInterface() {}

	unsigned int id{ 0 };

	void setType(ObjectType type)
	{
		this->type = type;
	}

private:
	// Static variable to keep track of the instance count to give each instance a unique ID
	static unsigned int instanceCount;
	ObjectType type{ NONE };
};
