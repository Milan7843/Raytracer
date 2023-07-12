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

void ImGuiUserInterface::drawUserInterface(GLFWwindow* window, SceneManager& sceneManager, Camera& camera, Renderer& renderer, ApplicationRenderMode& applicationRenderMode)
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

	bool showDemoWindow{ false };
	if (showDemoWindow)
	{
		ImGui::ShowDemoWindow();
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		return;
	}

	ImGuiWindowFlags window_flags{ 0 };
	window_flags |= ImGuiWindowFlags_MenuBar;
	window_flags |= ImGuiWindowFlags_NoNavInputs;
	window_flags |= ImGuiWindowFlags_NoNav;
	//window_flags |= ImGuiWindowFlags_Popup;
	bool windowOpen{ true };

	// Creating the GUI window
	ImGui::Begin("Editor", &windowOpen, window_flags);

	static bool updateSceneNames{ true };

	// Holds new scene name input
	static std::string newSceneNameInput{};
	static bool sceneNameInputError = false;
	bool openSaveAsPopup{ false };

	// Unselecting all objects: they will be marked as selected as needed later in this function
	//sceneManager.getCurrentScene().markAllUnselected();

	bool openHelpMenuButtonPressed{ false };
	// Menu Bar
	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("Scene"))
		{
			if (ImGui::BeginMenu("Open scene"))
			{
				// Showing all possible scene names
				for (std::string& name : sceneManager.getAvailableScenesNames(updateSceneNames))
				{
					// Making a button which loads the scene on click
					if (ImGui::MenuItem(name.c_str()))
					{
						// Loading the scene
						sceneManager.changeScene(name);
						break;
					}
				}

				updateSceneNames = false;

				ImGui::EndMenu();
			}
			else
			{
				updateSceneNames = true;
			}

			if (ImGui::MenuItem("New scene"))
			{
				sceneManager.newScene();
			}

			if (ImGui::MenuItem("Save"))
			{
				// If this scene does not yet have a name, open the save as popup
				if ((*sceneManager.getCurrentScene().getNamePointer()).empty())
					openSaveAsPopup = true;
				else
					sceneManager.saveChanges();
			}

			if (ImGui::MenuItem("Save as"))
			{
				openSaveAsPopup = true;
			}

			if (ImGui::MenuItem("Revert changes"))
			{
				sceneManager.revertChanges();
			}

			ImGui::EndMenu();
		}

		// Help menu
		static bool helpMenuWindowOpen{ false };

		if (ImGui::BeginMenu("Help"))
		{
			if (ImGui::MenuItem("Open help"))
			{
				std::cout << "menu clicked?" << std::endl;
				openHelpMenuButtonPressed = true;
			}

			ImGui::EndMenu();
		}

		ImGui::EndMenuBar();
	}

	if (openHelpMenuButtonPressed)
		ImGui::OpenPopup("##help_popup");

	if (ImGui::BeginPopup("##help_popup"))
	{
		drawHelpMenu();
		ImGui::EndPopup();
	}

	if (openSaveAsPopup)
		ImGui::OpenPopup("##save_changes_new_popup");

	if (ImGui::BeginPopup("##save_changes_new_popup"))
	{
		// Name input field
		ImGui::InputText("Scene name", &newSceneNameInput);

		if (sceneNameInputError)
			ImGui::Text("Invalid scene name. Make sure it does not contain periods ('.'), slashes ('/') or backslashes ('\\'),\n"
				"and it is not empty.");

		if (ImGui::Button("Save"))
		{
			if (FileUtility::isValidInput(newSceneNameInput))
			{
				if (sceneManager.willSaveOverwrite(newSceneNameInput))
				{
					ImGui::OpenPopup("##save_changes_overwrite_popup");
				}
				else
				{
					sceneManager.saveChangesAs(newSceneNameInput);
					// Empty input field
					newSceneNameInput = {};
					sceneNameInputError = false;

					// Then close the popup
					ImGui::CloseCurrentPopup();
				}
			}
			else
			{
				// Activating the error message
				sceneNameInputError = true;
			}
		}

		ImGui::SameLine();

		if (ImGui::Button("Cancel"))
		{
			// Empty input field
			newSceneNameInput = {};
			sceneNameInputError = false;

			// Then close the popup
			ImGui::CloseCurrentPopup();
		}

		// Indicates whether the 'save as' popup should be closed by an action of the 
		bool closeSaveSceneAsPopupAfterOverwritePopup{ false };

		// Popup to ask whether you want to overwrite a scene
		if (ImGui::BeginPopup("##save_changes_overwrite_popup"))
		{
			ImGui::Text(("Saving the scene with the name '" + newSceneNameInput
				+ "' will overwrite the scene with the same name.").c_str());
			ImGui::Text("Are you sure you want to overwrite this scene?");

			if (ImGui::Button("Save anyway"))
			{
				sceneManager.saveChangesAs(newSceneNameInput);

				// Empty input field
				newSceneNameInput = {};

				// Then close the popup
				sceneNameInputError = false;
				closeSaveSceneAsPopupAfterOverwritePopup = true;
				ImGui::CloseCurrentPopup();
			}
			ImGui::SameLine();
			if (ImGui::Button("Cancel"))
			{
				// Then close the popup
				sceneNameInputError = false;
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}

		if (closeSaveSceneAsPopupAfterOverwritePopup)
			ImGui::CloseCurrentPopup();

		ImGui::EndPopup();
	}

	ImGui::Text("Press TAB to open or close this interface.");

	/*
	if (ImGui::Button("Render frame"))
	{
		renderer.render();
	}
	*/
	if (ImGui::Button("Render frame in blocks"))
	{
		renderer.startBlockRender();
	}

	// Render time left indicators
	ImGui::ProgressBar(renderer.getRenderProgress());
	ImGui::Text("Render time left: ");
	ImGui::SameLine();
	ImGui::Text(formatTime(renderer.getTimeLeft()).c_str());

	static std::string renderSaveFileName{ "" };
	static bool renderSaveFileNameError = false;

	if (ImGui::Button("Save render"))
		ImGui::OpenPopup("##save_render_popup");

	if (ImGui::BeginPopup("##save_render_popup"))
	{
		// Name input field
		ImGui::InputText("Render name", &renderSaveFileName);
		if (renderSaveFileNameError)
			ImGui::Text("Invalid image name. Make sure it does not contain periods ('.'), slashes ('/') or backslashes ('\\'),\n"
				"and it is not empty.");

		if (ImGui::Button("Save"))
		{
			if (FileUtility::isValidInput(renderSaveFileName))
			{
				// Saving the render
				FileUtility::saveRender(renderSaveFileName + ".png", renderer.getWidth(), renderer.getHeight(), renderer.getPixelBuffer());

				// Empty input field
				renderSaveFileName = {};
				// Close error
				renderSaveFileNameError = false;
				// Then close the popup
				ImGui::CloseCurrentPopup();
			}
			else
			{
				// Activating the error
				renderSaveFileNameError = true;
			}
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel"))
		{
			// Empty input field
			renderSaveFileName = {};
			// Close error
			renderSaveFileNameError = false;
			// Then close the popup
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}


	// Creating the tab bar
	ImGui::BeginTabBar("full_tab_bar");


	ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.22f, 0.4f, 0.48f, 0.2f));

	// Tap for all settings related to rendering
	if (ImGui::BeginTabItem("Render settings"))
	{
		ImGui::BeginGroup();
		drawRenderSettings(sceneManager, camera, renderer, applicationRenderMode);
		ImGui::EndGroup();
		ImGui::EndTabItem();
	}

	// Tab for editing any scene objects
	if (ImGui::BeginTabItem("Scene editing"))
	{
		if (ImGui::Button("Set HDRI"))
			ImGui::OpenPopup("##open_hdri_popup");

		static bool updateHDRINames = true;

		if (ImGui::BeginPopup("##open_hdri_popup"))
		{
			// Stop updating while the popup is open
			updateHDRINames = false;

			// Showing all possible scene names
			for (std::string& name : sceneManager.getAvailableHDRINames(updateHDRINames))
			{
				// Making a button which loads the scene on click
				if (ImGui::Button(name.c_str()))
				{
					// Setting the HDRI
					sceneManager.loadHDRI(name);

					// Update next time
					updateHDRINames = true;

					// Closing the popup
					ImGui::CloseCurrentPopup();
					break;
				}
			}
			ImGui::EndPopup();
		}


		ImGui::BeginTabBar("scene_edit_tab_bar");
		ImGui::BeginGroup();

		/*
		ImGuiTabItemFlags_ objectsFlag{ ImGuiTabItemFlags_None };
		ImGuiTabItemFlags_ materialsFlag{ ImGuiTabItemFlags_None };
		ImGuiTabItemFlags_ lightsFlag{ ImGuiTabItemFlags_None };

		// Automatically open correct tab on selection
		if (newObjectSelected)
		{
			std::cout << selectedObjectType << std::endl;
			switch (selectedObjectType)
			{
				// Model or sphere: open the objects tab
				case 1: // model
				case 2: // sphere
					objectsFlag = ImGuiTabItemFlags_SetSelected;
					break;
			}
		}
		*/

		//if (ImGui::BeginTabItem("Objects", (bool*)0, objectsFlag))
		if (ImGui::BeginTabItem("Objects"))
		{
			drawObjects(sceneManager);
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Materials"))
		{
			drawMaterials(sceneManager.getCurrentScene());
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Lights"))
		{
			drawLights(sceneManager.getCurrentScene());
			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();
		ImGui::EndGroup();
		ImGui::EndTabItem();
	}

	// Camera settings (speed, fov etc.)
	if (ImGui::BeginTabItem("Camera settings"))
	{
		ImGui::SliderFloat("Move speed", camera.getCameraSpeedPointer(), 0.1f, 10.0f);
		ImGui::SliderFloat("Sensitivity", camera.getSensitivityPointer(), 0.1f, 5.0f);
		ImGui::SliderFloat("Field of view", camera.getFovPointer(), 10.0f, 90.0f);
		ImGui::EndTabItem();
	}
	ImGui::PopStyleColor();

	ImGui::EndTabBar();


	// Drawing the selected object
	ImGui::Separator();
	sceneManager.getCurrentScene().drawCurrentlySelectedObjectInterface();

	ImGui::End();

	// Rendering
	ImGui::Render();
	//int display_w, display_h;
	//glfwGetFramebufferSize(window, &display_w, &display_h);
	//glViewport(0, 0, display_w, display_h);
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void ImGuiUserInterface::handleInput(GLFWwindow* window, Camera& camera)
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
			camera.resetMouseOffset();
		}
	}

	guiSwitchKeyPreviousState = glfwGetKey(window, interfaceToggleKey);
}

bool ImGuiUserInterface::isEnabled()
{
	return imGuiEnabled;
}

bool ImGuiUserInterface::isMouseOnGUI()
{
	return ImGui::GetIO().WantCaptureMouse;
}

std::string ImGuiUserInterface::formatTime(float time)
{
	int secondsTotal = (int)time;
	int seconds = secondsTotal % 60;
	int minutes = (secondsTotal / 60) % 60;
	int hours = secondsTotal / 3600;

	return std::format("{}h {}m {}s", hours, minutes, seconds);
}

void ImGuiUserInterface::drawRenderSettings(SceneManager& sceneManager, Camera& camera, Renderer& renderer, ApplicationRenderMode& applicationRenderMode)
{
	renderer.drawInterface();

	ImGui::Checkbox("Use HDRI as background", sceneManager.getCurrentScene().getUseHDRIAsBackgroundPointer());
	ImGuiUtility::drawHelpMarker("Only if enabled, the HDRI will be drawn as the background.\nThe HDRI will be shown in reflections either way");

	int renderMode = (int)applicationRenderMode;
	// Button to switch between raytraced and rasterized views
    ImGui::RadioButton("Rasterized", &renderMode, 0); ImGui::SameLine();
    ImGui::RadioButton("Raytraced", &renderMode, 1); ImGui::SameLine();
    ImGui::RadioButton("Realtime raytraced", &renderMode, 2);
	applicationRenderMode = (ApplicationRenderMode)renderMode;
}

void ImGuiUserInterface::drawHelpMenu()
{
	ImGui::BeginTabBar("help_menu_tab_bar");

	ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);

	if (ImGui::BeginTabItem("Scene##scene_help_tab"))
	{
		ImGui::Text(
			"Everything exists inside one single scene at a time."
			"This means all models, spheres, lights, materials, the HDRI and the camera "
			"belong to the scene and will be saved and loaded with the scene."

		);
		if (ImGui::CollapsingHeader("Saving and loading"))
		{
			ImGui::Text(
				"To save a scene, press the 'Scene' button on the top bar, and select 'Save'."
				"This will save the scene to it's current name, which it will have if it was loaded in."
				"In case the scene does not yet have a name, you must choose one to have it saved by."
			);
			ImGui::Text(
				"By using the 'Save as' button in the same menu, the scene "
				"will be copied under another name, but you will stay in the same scene."
			);

			ImGui::Separator();

			ImGui::Text(
				"In order to load a scene from the disk, press the 'Scene' button on the top bar, then hover over 'Open scene'."
				"This will open a menu with the names of all scenes available on your disk."
				"To choose one simply press its name and it will be loaded as the current scene."
			);
		}
		if (ImGui::CollapsingHeader("Manually importing a scene"))
		{
			ImGui::Text(
				"In order to manually import a scene from another location, place a valid scene file in the 'scenes' folder."
				"It can then be loaded into the program using the usual loading method."
			);
		}
		if (ImGui::CollapsingHeader("Loading an HDRI"))
		{
			ImGui::Text(
				"In order to load an HDRI from the disk, navigate to the 'Scene editing' tab"
				"and press the button labeled 'Set HDRI'."
			);
			ImGui::Text(
				"If your HDRI does not show up here, make sure it is:"
			);
			ImGui::BulletText("An image file of PNG type.");
			ImGui::BulletText("Inside of the 'HDRIs' folder.");
		}
		ImGui::EndTabItem();
	}

	if (ImGui::BeginTabItem("Objects##objects_help_tab"))
	{
		ImGui::Text(
			"In order to do anything to the objects in the scene, navigate to the 'Scene editing' tab."
		);

		ImGui::Text(
			"From there, you can select any object by clicking on the correct tab, then clicking on it."
			"Doing this opens up an editor panel for the selected object below, and the selected object also receives an outline."
			"To edit any of the values in this editor panel, you can"
		);
		ImGui::BulletText("Click for a dropdown");
		ImGui::BulletText("Click and drag for a numerical value");
		ImGui::BulletText("Click and drag for a slider");

		ImGui::Text("Any numerical value van also be changed manually by holding down the Ctrl key and clicking it.");

		if (ImGui::CollapsingHeader("Materials"))
		{
			ImGui::Text(
				"These are the properties of the materials:"
			);
			ImGui::BulletText("Color: the base color of the material");
			ImGui::BulletText("Emission: not currently implemented");
			ImGui::BulletText("Reflectiveness: how much the material reflects light.");
			ImGui::BulletText("Transparency: how much the material lets light pass through.");
			ImGui::BulletText("Refractiveness: how much light is bent when passing through the material.");
			ImGui::BulletText("Reflective diffusion: how much light may be scattered on reflecting, which acts to blur the reflection.");
		}

		ImGui::EndTabItem();
	}

	if (ImGui::BeginTabItem("Camera##camera_help_tab"))
	{
		ImGui::Text(
			"There is always a single camera available to move around and look through."
			"Rendering is also done through this camera's perspective."
			"To change any of said camera's settings check out the 'Camera settings' tab in the 'Editor' panel."
		);
		if (ImGui::CollapsingHeader("Available settings"))
		{
			ImGui::BulletText("Sensitivity: how fast the camera rotates with mouse movement.");
			ImGui::BulletText("Movement speed: how fast the camera moves on keyboard input.");
			ImGui::BulletText("Field of view: the wideness of the lens in degrees.");
		}
		if (ImGui::CollapsingHeader("Moving the camera"))
		{
			ImGui::Text("To move the camera, use the following keys:");
			ImGui::BulletText("'W' to go forward.");
			ImGui::BulletText("'S' to go backward.");
			ImGui::BulletText("'A' to go left.");
			ImGui::BulletText("'D' to go right.");
			ImGui::BulletText("'Q' to go up.");
			ImGui::BulletText("'E' to go down.");
			ImGui::Text("Additionaly, you can hold the shift key to temporarily increase the camera's speed.");
		}
		ImGui::EndTabItem();
	}

	ImGui::PopTextWrapPos();

	ImGui::EndTabBar();
}

void ImGuiUserInterface::drawMaterials(Scene& scene)
{
	ImGui::PushItemWidth(-1);
	if (ImGui::BeginListBox("##"))
	{
		int index = 0;
		for (Material& material : scene.getMaterials())
		{
			// Skip the first material: it is the NONE material used if none are selected on an object
			if (index == 0)
			{
				index++;
				continue;
			}

			// Drawing each material
			drawMaterial(material, scene, index);
			index++;
		}

		// Drawing the 'Add material' button
		if (ImGui::Button("Add material"))
		{
			Material material;
			scene.addMaterial(material);
		}

		ImGui::EndListBox();
	}
	ImGui::PopItemWidth();
}

void ImGuiUserInterface::drawMaterial(Material& material, Scene& scene, unsigned int index)
{
	// Get the material name, then add a constant ID so that the 
	// ID doesn't have to change when the material's name changes
	std::string popupID{
		(*material.getNamePointer())
		+ "###material"
		+ std::to_string(index)
	};

	if (ImGui::Button(popupID.c_str()))
		scene.markSelected(ObjectType::MATERIAL, index);

	// Drawing the 'delete' button
	if (ImGui::BeginPopupContextItem())
	{
		if (ImGui::Button("Delete"))
		{
			scene.deleteMaterial(index);
		}
		ImGui::EndPopup();
	}
}

void ImGuiUserInterface::drawLights(Scene& scene)
{
	ImGui::PushItemWidth(-1);
	ImGui::BeginListBox("##");
	unsigned int index = 0;

	// Drawing the lights themselves
	for (PointLight& light : scene.getPointLights())
	{
		drawLight(light, scene, index);
		index++;
	}

	ImGui::Separator();

	index = 0;
	for (DirectionalLight& light : scene.getDirectionalLights())
	{
		drawLight(light, scene, index);
		index++;
	}

	ImGui::Separator();

	index = 0;
	for (AmbientLight& light : scene.getAmbientLights())
	{
		drawLight(light, scene, index);
		index++;
	}

	// Drawing the 'Add light' button
	if (ImGui::Button("Add light"))
		ImGui::OpenPopup("add_light_popup");

	// And drawing the light options
	if (ImGui::BeginPopup("add_light_popup"))
	{
		ImGui::Text("Light type");
		ImGui::Separator();
		if (ImGui::Selectable("Point light"))
		{
			PointLight light(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), 1.0f, 0.0f);
			scene.addLight(light);
		}
		if (ImGui::Selectable("Directional light"))
		{
			DirectionalLight light(glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), 1.0f, 0.0f);
			scene.addLight(light);
		}
		if (ImGui::Selectable("Ambient light"))
		{
			AmbientLight light(glm::vec3(1.0f, 1.0f, 1.0f), 0.1f);
			scene.addLight(light);
		}
		ImGui::EndPopup();
	}
	ImGui::EndListBox();
	ImGui::PopItemWidth();	
}

void ImGuiUserInterface::drawLight(PointLight& light, Scene& scene, unsigned int index)
{
	// Get the light name, then add a constant ID so that the 
	// ID doesn't have to change when the light's name changes
	std::string popupID{
		light.getName()
		+ "###point_light_tree_node"
		+ std::to_string(index)
	};

	if (ImGui::Button(popupID.c_str()))
		scene.markSelected(ObjectType::POINT_LIGHT, index);

	if (ImGui::BeginPopupContextItem())
	{
		if (ImGui::Button("Delete"))
		{
			scene.deletePointLight(index);
		}
		ImGui::EndPopup();
	}
}

void ImGuiUserInterface::drawLight(DirectionalLight& light, Scene& scene, unsigned int index)
{
	// Get the light name, then add a constant ID so that the 
	// ID doesn't have to change when the light's name changes
	std::string popupID {
		light.getName()
		+ "###dir_light_tree_node"
		+ std::to_string(index)
	};

	if (ImGui::Button(popupID.c_str()))
		scene.markSelected(ObjectType::DIRECTIONAL_LIGHT, index);

	if (ImGui::BeginPopupContextItem())
	{
		if (ImGui::Button("Delete"))
		{
			scene.deleteDirectionalLight(index);
		}
		ImGui::EndPopup();
	}
}

void ImGuiUserInterface::drawLight(AmbientLight& light, Scene& scene, unsigned int index)
{
	// Get the light name, then add a constant ID so that the 
	// ID doesn't have to change when the light's name changes
	std::string popupID {
		light.getName()
		+ "###ambient_light_tree_node"
		+ std::to_string(index)
	};

	if (ImGui::Button(popupID.c_str()))
		scene.markSelected(ObjectType::AMBIENT_LIGHT, index);

	if (ImGui::BeginPopupContextItem())
	{
		if (ImGui::Button("Delete"))
		{
			scene.deleteAmbientLight(index);
		}
		ImGui::EndPopup();
	}
}

void ImGuiUserInterface::drawObjects(SceneManager& sceneManager)
{
	// Generating the char[] used for the material slots
	std::string materialSlots = "";
	for (Material& material : sceneManager.getCurrentScene().getMaterials())
	{
		materialSlots += *material.getNamePointer() + "\000";
	}
	const char* materialSlotsCharArray = materialSlots.c_str();

	// Drawing the models

	if (ImGui::TreeNode("Models"))
	{
		ImGui::PushItemWidth(-1);
		if (ImGui::BeginListBox("##"))
		{
			unsigned int index = 0;
			for (Model& model : sceneManager.getCurrentScene().getModels())
			{
				// Drawing each model
				drawObject(model, sceneManager.getCurrentScene(), index, materialSlotsCharArray);
				index++;
			}

			if (ImGui::Button("Add model"))
				ImGui::OpenPopup("##add_model_popup");

			static bool updateModelNames{ true };

			if (ImGui::BeginPopup("##add_model_popup"))
			{
				// Showing all possible model names
				for (std::string& name : sceneManager.getAvailableModelsNames(updateModelNames))
				{
					// Making a button which loads the scene on click
					if (ImGui::MenuItem(name.c_str()))
					{
						// Loading the scene
						sceneManager.getCurrentScene().addModel("src/models/" + name + ".obj", 0);
						sceneManager.getCurrentScene().generateTriangleBuffer();
						break;
					}
				}

				updateModelNames = false;

				ImGui::EndPopup();
			}
			else
			{
				// Mark for update again after closing
				updateModelNames = true;
			}

			ImGui::EndListBox();
		}
		ImGui::PopItemWidth();
		ImGui::TreePop();
	}

	// Drawing the spheres

	if (ImGui::TreeNode("Spheres"))
	{
		ImGui::PushItemWidth(-1);
		if (ImGui::BeginListBox("##"))
		{
			unsigned int index = 0;
			for (Sphere& sphere : sceneManager.getCurrentScene().getSpheres())
			{
				// Drawing each sphere
				drawObject(sphere, sceneManager.getCurrentScene(), index, materialSlotsCharArray);
				index++;
			}

			// Drawing the 'Add sphere' button
			if (ImGui::Button("Add sphere"))
				sceneManager.getCurrentScene().addSphere(glm::vec3(0.0f), 1.0f, 0);

			ImGui::EndListBox();
		}
		ImGui::PopItemWidth();
		ImGui::TreePop();
	}
}

void ImGuiUserInterface::drawObject(Model& object, Scene& scene, unsigned int index, const char* materialSlotsCharArray)
{
	// Get the object's name, then add a constant ID so that the 
	// ID doesn't have to change when the light's name changes
	std::string popupID {
		object.getName()
		+ "###model"
		+ std::to_string(index)
	};

	if (ImGui::Button(popupID.c_str()))
		scene.markSelected(ObjectType::MODEL, index);

	if (ImGui::BeginPopupContextItem())
	{
		if (ImGui::Button("Delete"))
		{
			scene.deleteModel(index);
		}
		ImGui::EndPopup();
	}
}

void ImGuiUserInterface::drawMesh(Mesh& object, Scene& scene, const char* materialSlotsCharArray, unsigned int index)
{
	unsigned int i = 0;

	// Preview the currently selected name
	if (ImGui::BeginCombo((object.getName() + "##combo").c_str(), (*(scene.getMaterials()[*object.getMaterialIndexPointer()].getNamePointer())).c_str()))
	{
		// Looping over each material to check whether it was clicked;
		// If it was: select the index of the material as the material index for this mesh
		for (Material& material : scene.getMaterials())
		{
			bool thisMaterialSelected = (i == *object.getMaterialIndexPointer());

			// Button for selecting material
			if (ImGui::Selectable((*(scene.getMaterials()[i].getNamePointer())).c_str()))
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
}



void ImGuiUserInterface::drawObject(Sphere& object, Scene& scene, unsigned int index, const char* materialSlotsCharArray)
{
	// Get the object's name, then add a constant ID so that the 
	// ID doesn't have to change when the light's name changes
	std::string popupID {
		object.getName()
		+ "###sphere"
		+ std::to_string(index)
	};

	if (ImGui::Button(popupID.c_str()))
		scene.markSelected(ObjectType::SPHERE, index);

	if (ImGui::BeginPopupContextItem())
	{
		if (ImGui::Button("Delete"))
		{
			scene.deleteSphere(index);
		}
		ImGui::EndPopup();
	}
}
