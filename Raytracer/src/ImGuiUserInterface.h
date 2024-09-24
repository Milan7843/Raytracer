#pragma once

#include <format>
#include <iostream>
#include <string>

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

#include "ImGuiUtility.h"
#include "Scene.h"
#include "SceneManager.h"
#include "Camera.h"
#include "Renderer.h"
#include "FileUtility.h"
#include "ApplicationRenderMode.h"
#include "RasterizedDebugMode.h"
#include "gui/ContextMenuSource.h"
#include "InputManager.h"

class ImGuiUserInterface
{
public:
	ImGuiUserInterface();
	~ImGuiUserInterface();


	// Initiliase everything required for ImGui
	void initialiseImGui(GLFWwindow * window);

	// Draw the user interface
	void drawUserInterface(GLFWwindow* window,
		SceneManager& sceneManager,
		Camera& camera,
		Renderer& renderer,
		ApplicationRenderMode& applicationRenderMode,
		RasterizedDebugMode& rasterizedDebugMode,
		ContextMenuSource* contextMenuSource,
		unsigned int screenTexture);

	void handleInput(GLFWwindow* window, Camera& camera);

	bool isEnabled() const;

	// Get whether the mouse is currently on the GUI
	bool isMouseOnGUI();

	// Get whether the mouse is currently on the rendered screen
	bool isMouseOnRenderedScreen() const;

	// Get the position of the mouse ([0,1], [0,1])
	glm::vec2 getMousePosition() const;

	// Get the coordinates of the mouse (pixel index x and y)
	glm::ivec2 getMouseCoordinates() const;

	// Get the size of the screen we are rendering to
	glm::ivec2 getRenderedScreenSize() const;

	bool isExitOkay() const;
	void requestExit();

private:
	bool imGuiEnabled = true;
	unsigned int guiSwitchKeyPreviousState = 0;

	bool exitRequested{ false };
	bool exitOkay{ false };

	unsigned int interfaceToggleKey = GLFW_KEY_TAB;

	glm::ivec2 mouseCoordinates{ glm::ivec2(0) };
	glm::vec2 mousePosition{ glm::vec2(0) };
	glm::ivec2 renderedScreenSize{ glm::ivec2(0) };

	// Transformation saved variables
	bool transformationFirstFrame{ true };
	glm::mat4 transformationInitialObjectMatrix;
	glm::mat4 transformationInitialMeshMatrix;
	unsigned int currentTransformationObjectID;
	bool previousFrameTransformationDragging{ false };
	bool transformationCancelled{ false };

	void drawGUI(GLFWwindow* window,
		SceneManager& sceneManager,
		Camera& camera,
		Renderer& renderer,
		ApplicationRenderMode& applicationRenderMode,
		ContextMenuSource* contextMenuSource);

	// Format a number of seconds
	std::string formatTime(float time);

	// Draw all the render settings
	void drawRenderSettings(SceneManager& sceneManager, Camera& camera, Renderer& renderer, ApplicationRenderMode& applicationRenderMode);

	// Draw the help menu window
	void drawHelpMenu();

	void drawSceneSelector(GLFWwindow* window,
		SceneManager& sceneManager,
		Camera& camera,
		Renderer& renderer,
		ApplicationRenderMode& applicationRenderMode,
		ContextMenuSource* contextMenuSource);

	void drawSceneEditor(GLFWwindow* window,
		SceneManager& sceneManager,
		Camera& camera,
		Renderer& renderer,
		ApplicationRenderMode& applicationRenderMode,
		ContextMenuSource* contextMenuSource);

	// Draw the main menu bar
	void drawMenuBar(GLFWwindow* window,
		SceneManager& sceneManager,
		Camera& camera,
		Renderer& renderer,
		ApplicationRenderMode& applicationRenderMode,
		RasterizedDebugMode& rasterizedDebugMode,
		ContextMenuSource* contextMenuSource);

	void drawExitWithUnsavedChangesPrompt(GLFWwindow* window,
		SceneManager& sceneManager,
		Camera& camera,
		Renderer& renderer,
		ApplicationRenderMode& applicationRenderMode,
		RasterizedDebugMode& rasterizedDebugMode,
		ContextMenuSource* contextMenuSource);

	void drawRenderInfo(GLFWwindow* window,
		SceneManager& sceneManager,
		Camera& camera,
		Renderer& renderer,
		ApplicationRenderMode& applicationRenderMode,
		RasterizedDebugMode& rasterizedDebugMode,
		ContextMenuSource* contextMenuSource);

	// Represent a material using ImGui
	void drawMaterials(Scene& scene);
	void drawMaterial(Material& material, Scene& scene, unsigned int index);

	// Represent a point light using ImGui
	void drawLights(Scene& scene);
	virtual void drawLight(PointLight& light, Scene& scene, unsigned int index);
	virtual void drawLight(DirectionalLight& light, Scene& scene, unsigned int index);
	virtual void drawLight(AmbientLight& light, Scene& scene, unsigned int index);

	// Represent a directional light using ImGui
	//void drawLight(PointLight& light);

	// Represent an object using ImGui
	void drawObjects(SceneManager& sceneManager);
	void drawObject(Model& object, Scene& scene, unsigned int index, const char* materialSlotsCharArray);
	void drawMesh(Mesh& object, Scene& scene, const char* materialSlotsCharArray, unsigned int index);
	void drawObject(Sphere& object, Scene& scene, unsigned int index, const char* materialSlotsCharArray);
};