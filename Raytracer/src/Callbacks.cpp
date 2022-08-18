#include "Callbacks.h"

SceneManager* Callbacks::sceneManager;

void Callbacks::bindSceneManager(SceneManager* sceneManager)
{
    Callbacks::sceneManager = sceneManager;
}

Callbacks& Callbacks::getInstance()
{
    static Callbacks instance;
    return instance;
}

void Callbacks::framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    getInstance().framebuffer_size_callbackImpl(window, width, height);
}

void Callbacks::framebuffer_size_callbackImpl(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void Callbacks::mouseCallback(GLFWwindow* window, double xpos, double ypos)
{
    getInstance().mouseCallbackImpl(window, xpos, ypos);
}

void Callbacks::mouseCallbackImpl(GLFWwindow* window, double xpos, double ypos)
{
    //sceneManager->getCurrentScene().getActiveCamera().mouseCallback(window, xpos, ypos);
}