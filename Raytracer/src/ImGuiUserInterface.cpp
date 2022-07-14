#pragma once

#include "ImGuiUserInterface.h"

ImGuiUserInterface::ImGuiUserInterface()
{
}

ImGuiUserInterface::~ImGuiUserInterface()
{
}

void ImGuiUserInterface::initialiseImGui(GLFWwindow* window)
{
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style to be dark
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");
}

void ImGuiUserInterface::drawUserInterface(Scene* scene, Camera* camera, Renderer* renderer, bool* inRaytraceMode)
{
	if (!imGuiEnabled)
	{
		// Don't draw the user interface
		return;
	}

	// Start the Dear ImGui frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	// Creating the GUI window
	ImGui::Begin("Render settings");

	ImGui::Text("Press R to open or close this interface.");

	if (ImGui::Button("Render frame"))
	{
		renderer->render(scene, camera);
	}
	if (ImGui::Button("Render frame in blocks"))
	{
		renderer->startBlockRender(scene, camera);
	}
	ImGui::ProgressBar(renderer->getRenderProgress());
	ImGui::SliderInt("Block size", renderer->getBlockSizePointer(), 1, 400);

	// Button to switch between raytraced and rasterized views
	if (ImGui::Button(*inRaytraceMode ? "View rasterized" : "View raytraced"))
	{
		*inRaytraceMode = !(*inRaytraceMode);
	}

	ImGui::Separator();

	// Camera settings (speed, fov etc.)
	if (ImGui::CollapsingHeader("Camera settings"))
	{
		ImGui::SliderFloat("Move speed", camera->getCameraSpeedPointer(), 0.1f, 10.0f);
		ImGui::SliderFloat("Sensitivity", camera->getSensitivityPointer(), 0.1f, 5.0f);
		ImGui::SliderFloat("Field of view", camera->getFovPointer(), 10.0f, 90.0f);
	}

	// Customisation of the program (colors etc.)
	if (ImGui::CollapsingHeader("Customisation"))
	{
		//ImGui::ColorEdit3("Background colour", (float*)&clearColor);
		//ImGui::ColorEdit3("Upper graph colour", (float*)&upperColor);
		//ImGui::ColorEdit3("Lower graph colour", (float*)&lowerColor);
	}

	ImGui::End();

	// Rendering
	ImGui::Render();
	//int display_w, display_h;
	//glfwGetFramebufferSize(window, &display_w, &display_h);
	//glViewport(0, 0, display_w, display_h);
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void ImGuiUserInterface::handleInput(GLFWwindow* window, Camera* camera)
{
	// Enable/disable the ImGui GUI on key switch
	if (glfwGetKey(window, interfaceToggleKey) == GLFW_PRESS && guiSwitchKeyPreviousState == GLFW_RELEASE)
	{
		imGuiEnabled = !imGuiEnabled;

		// If the GUI is enabled, 
		if (imGuiEnabled)
		{
			// free the mouse
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		}
		// Otherwise,
		else
		{
			// Lock the mouse
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			// Reset offset (the mouse may have moved which should not be registered as camera movement)
			camera->resetMouseOffset();
		}
	}

	guiSwitchKeyPreviousState = glfwGetKey(window, interfaceToggleKey);
}

bool ImGuiUserInterface::isEnabled()
{
	return imGuiEnabled;
}

