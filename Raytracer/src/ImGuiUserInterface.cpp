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

	// Drawing all settings related to rendering
	if (ImGui::CollapsingHeader("Render settings"))
	{
		drawRenderSettings(camera, renderer, inRaytraceMode);
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

	drawMaterials(scene);
	drawLights(scene);

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

void ImGuiUserInterface::drawRenderSettings(Camera* camera, Renderer* renderer, bool* inRaytraceMode)
{
	ImGui::Text(std::to_string(renderer->getTimeLeft()).c_str());

	ImGui::SliderInt("Block size", renderer->getBlockSizePointer(), 1, 100);
	if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
	{
		ImGui::SetTooltip("The size of a render block in pixels.");
	}

	ImGui::SliderInt("Multisamples", renderer->getMultisamplePointer(), 1, 5);
	if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
	{
		ImGui::SetTooltip("The number of different sample points per pixel, works as anti-aliasing.");
	}
	ImGui::Text(camera->getInformation().c_str());

	// Samples per render pass
	ImGui::SliderInt("Sample count", renderer->getSampleCountPointer(), 1, 100);
	if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
	{
		ImGui::SetTooltip("The number of samples per pixel per render pass.");
	}

	// Render passes per block
	ImGui::SliderInt("Block passes", renderer->getRenderPassCountPointer(), 1, 100);
	if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
	{
		ImGui::SetTooltip("The number of passes per block. Each pass will take the full number of samples for each pixel.");
	}

	// Button to switch between raytraced and rasterized views
	if (ImGui::Button(*inRaytraceMode ? "View rasterized" : "View raytraced"))
	{
		*inRaytraceMode = !(*inRaytraceMode);
	}
}

void ImGuiUserInterface::drawMaterials(Scene* scene)
{
	if (ImGui::CollapsingHeader("Materials"))
	{
		for (Material& material : scene->getMaterials())
		{
			// Drawing each material
			drawMaterial(material);
			material.refractiveness = 1.0f;
		}
	}
}

void ImGuiUserInterface::drawMaterial(Material& material)
{
	if (ImGui::TreeNode((*material.getNamePointer()).c_str()))
	{
		ImGui::InputText("Name", material.getNamePointer());
		ImGui::ColorEdit3("Color", (float*)material.getColorPointer());
		ImGui::ColorEdit3("Emission", (float*)material.getEmissionPointer());
		ImGui::DragFloat("Reflectiveness", material.getReflectivenessPointer(), 0.01f, 0.0f, 1.0f, "%.2f");
		ImGui::DragFloat("Transparency", material.getTransparencyPointer(), 0.01f, 0.0f, 1.0f, "%.2f");
		ImGui::DragFloat("Refractiveness", material.getRefractivenessPointer(), 0.01f, 0.0f, 1.0f, "%.2f");
		ImGui::TreePop();
		ImGui::Separator();
	}
}

void ImGuiUserInterface::drawLights(Scene* scene)
{
	if (ImGui::CollapsingHeader("Lights"))
	{
		unsigned int index = 0;
		for (PointLight& light : scene->getPointLights())
		{
			// Drawing each point light
			drawLight(light, index);
			index++;
		}
	}
}

void ImGuiUserInterface::drawLight(PointLight& light, unsigned int index)
{
	if (ImGui::TreeNode(("Point light " + std::to_string(index + 1)).c_str()))
	{
		ImGui::ColorEdit3("Color", (float*)light.getColorPointer());
		ImGui::DragFloat("Intensity", light.getIntensityPointer(), 0.01f, 0.0f, 10.0f, "%.2f");
		ImGui::DragFloat3("Position", (float*)light.getPositionPointer(), 0.01f);
		ImGui::TreePop();
		ImGui::Separator();
	}
}

void ImGuiUserInterface::drawObjects(Scene* scene)
{
}

void ImGuiUserInterface::drawObject(Object& object)
{
}

