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

#include "Scene.h"
#include "Camera.h"
#include "Renderer.h"

class ImGuiUserInterface
{
public:
	ImGuiUserInterface();
	~ImGuiUserInterface();


	// Initiliase everything required for ImGui
	void initialiseImGui(GLFWwindow * window);

	// Draw the user interface
	void drawUserInterface(Scene * scene, Camera * camera, Renderer * renderer, bool* inRaytraceMode);

	void handleInput(GLFWwindow* window, Camera* camera);

	bool isEnabled();

private:
	bool imGuiEnabled = true;
	unsigned int guiSwitchKeyPreviousState = 0;

	unsigned int interfaceToggleKey = GLFW_KEY_R;

	// Draw all the render settings
	void drawRenderSettings(Camera* camera, Renderer* renderer, bool* inRaytraceMode);

	// Represent a material using ImGui
	void drawMaterials(Scene* scene);
	void drawMaterial(Material& material);

	// Represent a point light using ImGui
	void drawLights(Scene* scene);
	void drawLight(PointLight& light, unsigned int index);

	// Represent a directional light using ImGui
	//void drawLight(PointLight& light);

	// Represent an object using ImGui
	void drawObjects(Scene* scene);
	void drawObject(Object& object);
};