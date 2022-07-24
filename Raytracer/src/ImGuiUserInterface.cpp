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

	bool showDemoWindow = false;
	if (showDemoWindow)
	{
		ImGui::ShowDemoWindow();
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		return;
	}

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

	// Render time left indicators
	ImGui::ProgressBar(renderer->getRenderProgress());
	ImGui::Text("Render time left: ");
	ImGui::SameLine();
	ImGui::Text(formatTime(renderer->getTimeLeft()).c_str());

	// Creating the tab bar
	ImGui::BeginTabBar("full_tab_bar");


	ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.22f, 0.4f, 0.48f, 0.2f));

	// Tap for all settings related to rendering
	if (ImGui::BeginTabItem("Render settings"))
	{
		ImGui::BeginGroup();
		drawRenderSettings(camera, renderer, inRaytraceMode);
		ImGui::EndGroup();
		ImGui::EndTabItem();
	}

	// Tab for editing any scene objects
	if (ImGui::BeginTabItem("Scene editing"))
	{
		ImGui::BeginTabBar("scene_edit_tab_bar");
		ImGui::BeginGroup();

		if (ImGui::BeginTabItem("Objects"))
		{
			drawObjects(scene);
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Materials"))
		{
			drawMaterials(scene);
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Lights"))
		{
			drawLights(scene);
			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();
		ImGui::EndGroup();
		ImGui::EndTabItem();
	}

	ImGui::Separator();

	// Camera settings (speed, fov etc.)
	if (ImGui::BeginTabItem("Camera settings"))
	{
		ImGui::SliderFloat("Move speed", camera->getCameraSpeedPointer(), 0.1f, 10.0f);
		ImGui::SliderFloat("Sensitivity", camera->getSensitivityPointer(), 0.1f, 5.0f);
		ImGui::SliderFloat("Field of view", camera->getFovPointer(), 10.0f, 90.0f);
		ImGui::EndTabItem();
	}
	ImGui::PopStyleColor();

	ImGui::EndTabBar();

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

std::string ImGuiUserInterface::formatTime(float time)
{
	int secondsTotal = (int)time;
	int seconds = secondsTotal % 60;
	int minutes = (secondsTotal / 60) % 60;
	int hours = secondsTotal / 3600;

	return std::format("{}h {}m {}s", hours, minutes, seconds);
}

void ImGuiUserInterface::drawHelpMarker(const char* desc)
{
	// Draw the help marker after whatever has already been drawn on this line
	ImGui::SameLine();

	// Gray text [?]
	ImGui::TextDisabled("[?]");

	// Drawing the help marker on hover
	if (ImGui::IsItemHovered())
	{
		ImGui::BeginTooltip();
		ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
		ImGui::TextUnformatted(desc);
		ImGui::PopTextWrapPos();
		ImGui::EndTooltip();
	}
}

void ImGuiUserInterface::drawRenderSettings(Camera* camera, Renderer* renderer, bool* inRaytraceMode)
{
	ImGui::SliderInt("Block size", renderer->getBlockSizePointer(), 1, 100);
	drawHelpMarker("The size of a render block in pixels.");

	ImGui::SliderInt("Multisamples", renderer->getMultisamplePointer(), 1, 5);
	drawHelpMarker("The number of different sample points per pixel, works as anti-aliasing.");
	ImGui::Text(camera->getInformation().c_str());

	// Samples per render pass
	ImGui::SliderInt("Sample count", renderer->getSampleCountPointer(), 1, 100);
	drawHelpMarker("The number of samples per pixel per render pass.");

	// Render passes per block
	ImGui::SliderInt("Block passes", renderer->getRenderPassCountPointer(), 1, 100);
	drawHelpMarker("The number of passes per block. Each pass will take the full number of samples for each pixel.");

	// Button to switch between raytraced and rasterized views
	if (ImGui::Button(*inRaytraceMode ? "View rasterized" : "View raytraced"))
	{
		*inRaytraceMode = !(*inRaytraceMode);
	}
}

void ImGuiUserInterface::drawMaterials(Scene* scene)
{
	ImGui::PushItemWidth(-1);
	ImGui::BeginListBox("##");
	for (Material& material : scene->getMaterials())
	{
		// Drawing each material
		drawMaterial(material);
		material.refractiveness = 1.0f;
	}
	ImGui::EndListBox();
	ImGui::PopItemWidth();
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
	ImGui::PushItemWidth(-1);
	ImGui::BeginListBox("##");
	unsigned int index = 0;
	for (PointLight& light : scene->getPointLights())
	{
		drawLight(light, index);
		index++;
	}
	index = 0;
	for (DirectionalLight& light : scene->getDirectionalLights())
	{
		drawLight(light, index);
		index++;
	}
	index = 0;
	for (AmbientLight& light : scene->getAmbientLights())
	{
		drawLight(light, index);
		index++;
	}
	ImGui::EndListBox();
	ImGui::PopItemWidth();	
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

void ImGuiUserInterface::drawLight(DirectionalLight& light, unsigned int index)
{
	if (ImGui::TreeNode(("Directional light " + std::to_string(index + 1)).c_str()))
	{
		ImGui::ColorEdit3("Color", (float*)light.getColorPointer());
		ImGui::DragFloat("Intensity", light.getIntensityPointer(), 0.01f, 0.0f, 10.0f, "%.2f");
		ImGui::DragFloat3("Direction", (float*)light.getDirectionPointer(), 0.01f);
		ImGui::TreePop();
		ImGui::Separator();
	}
}

void ImGuiUserInterface::drawLight(AmbientLight& light, unsigned int index)
{
	if (ImGui::TreeNode(("Ambient light " + std::to_string(index + 1)).c_str()))
	{
		ImGui::ColorEdit3("Color", (float*)light.getColorPointer());
		ImGui::DragFloat("Intensity", light.getIntensityPointer(), 0.01f, 0.0f, 10.0f, "%.2f");
		ImGui::TreePop();
		ImGui::Separator();
	}
}

void ImGuiUserInterface::drawObjects(Scene* scene)
{
	// Generating the char[] used for the material slots
	std::string materialSlots = "";
	for (Material& material : scene->getMaterials())
	{
		materialSlots += *material.getNamePointer() + "\000";
	}
	const char* materialSlotsCharArray = materialSlots.c_str();

	// Drawing the models
	if (ImGui::TreeNode("Models"))
	{
		ImGui::PushItemWidth(-1);
		ImGui::BeginListBox("##");
		unsigned int index = 0;
		for (Model& model : scene->getModels())
		{
			// Drawing each model
			drawObject(model, scene, index, materialSlotsCharArray);
			index++;
		}
		ImGui::EndListBox();
		ImGui::PopItemWidth();
		ImGui::TreePop();
	}
	// Drawing the spheres
	if (ImGui::TreeNode("Spheres"))
	{
		ImGui::PushItemWidth(-1);
		ImGui::BeginListBox("##");
		unsigned int index = 0;
		for (Sphere& sphere : scene->getSpheres())
		{
			// Drawing each sphere
			drawObject(sphere, scene, index, materialSlotsCharArray);
			index++;
		}
		ImGui::EndListBox();
		ImGui::PopItemWidth();
		ImGui::TreePop();
	}
}

void ImGuiUserInterface::drawObject(Object& object, Scene* scene, unsigned int index, const char* materialSlotsCharArray)
{
	if (ImGui::TreeNode(("Model " + std::to_string(index + 1)).c_str()))
	{
		// Showing transformations
		ImGui::DragFloat3("Position", (float*)object.getPositionPointer(), 0.01f);
		ImGui::DragFloat3("Rotation", (float*)object.getRotationPointer(), 0.01f);
		ImGui::DragFloat3("Scale", (float*)object.getScalePointer(), 0.01f);

		// Preview the currently selected name
		if (ImGui::BeginCombo("##combo", (*(scene->getMaterials()[*object.getMaterialIndexPointer()].getNamePointer())).c_str()))
		{
			unsigned int i = 0;
			for (Material& material : scene->getMaterials())
			{
				bool thisMaterialSelected = (i == *object.getMaterialIndexPointer());
				
				if (ImGui::Selectable((*(scene->getMaterials()[i].getNamePointer())).c_str()))
				{
					*object.getMaterialIndexPointer() = i;
				}

				if (thisMaterialSelected)
				{
					ImGui::SetItemDefaultFocus();
				}

				// Increment material counter
				i++;
			}

			// End this combo selector
			ImGui::EndCombo();
		}

		ImGui::TreePop();
		ImGui::Separator();
	}
}



void ImGuiUserInterface::drawObject(Sphere& object, Scene* scene, unsigned int index, const char* materialSlotsCharArray)
{
	if (ImGui::TreeNode(("Sphere " + std::to_string(index + 1)).c_str()))
	{
		// Showing transformations
		ImGui::DragFloat3("Position", (float*)object.getPositionPointer(), 0.01f);

		// Drawing a dragfloat for the sphere's radius
		Sphere& sphere = (Sphere&)object;
		ImGui::DragFloat("Radius", sphere.getRadiusPointer());

		// Preview the currently selected name
		if (ImGui::BeginCombo("##combo", (*(scene->getMaterials()[*object.getMaterialIndexPointer()].getNamePointer())).c_str()))
		{
			unsigned int i = 0;
			for (Material& material : scene->getMaterials())
			{
				bool thisMaterialSelected = (i == *object.getMaterialIndexPointer());

				if (ImGui::Selectable((*(scene->getMaterials()[i].getNamePointer())).c_str()))
				{
					*object.getMaterialIndexPointer() = i;
				}

				if (thisMaterialSelected)
				{
					ImGui::SetItemDefaultFocus();
				}

				// Increment material counter
				i++;
			}

			// End this combo selector
			ImGui::EndCombo();
		}

		ImGui::TreePop();
		ImGui::Separator();
	}
}