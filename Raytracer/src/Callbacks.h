#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>

// Matrix math
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "SceneManager.h"

class Callbacks
{
public:
    void bindSceneManager(SceneManager* sceneManager);

    static Callbacks& getInstance();

    static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
    void framebuffer_size_callbackImpl(GLFWwindow* window, int width, int height);
    static void mouseCallback(GLFWwindow* window, double xpos, double ypos);
    void mouseCallbackImpl(GLFWwindow* window, double xpos, double ypos);
    static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
    void scrollCallbackImpl(GLFWwindow* window, double xoffset, double yoffset);

private:
    static SceneManager* sceneManager;

    Callbacks(void)
    {
    }

    Callbacks(Callbacks const&);
    void operator=(Callbacks const&);
};