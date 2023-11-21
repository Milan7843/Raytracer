#include "WindowUtility.h"

namespace WindowUtility
{
    namespace
    {
        GLFWwindow* savedWindow;
        std::string currentWindowName{ "" };
        bool unsavedChanges{ false };

        const std::string newSceneWindowName{ "New Scene" };
    }

    void setWindow(GLFWwindow* window)
    {
        savedWindow = window;
    }

    void setWindowTitle(const std::string& title)
    {
        currentWindowName = title;
        glfwSetWindowTitle(savedWindow, title.c_str());
    }

    void setWindowTitleNewScene()
    {
        setWindowTitle("New Scene");
    }

    void markUnsavedChanges()
    {
        // Check if it already marked as unsaved changes
        if (unsavedChanges)
        {
            return;
        }

        unsavedChanges = true;
        glfwSetWindowTitle(savedWindow, (currentWindowName + "*").c_str());
    }

    void markSavedChanges()
    {
        // Check if it already marked as no unsaved changes
        if (!unsavedChanges)
        {
            return;
        }

        unsavedChanges = false;
        glfwSetWindowTitle(savedWindow, currentWindowName.c_str());
    }

    std::string openImageFileChooseDialog()
    {
        OPENFILENAME ofn;
        TCHAR szFile[260] = { 0 };

        std::filesystem::path initialDir = std::filesystem::current_path();

        ZeroMemory(&ofn, sizeof(ofn));
        ofn.lStructSize = sizeof(ofn);
        ofn.hwndOwner = glfwGetWin32Window(savedWindow);
        ofn.lpstrFile = szFile;
        ofn.nMaxFile = sizeof(szFile);
        ofn.lpstrFilter = TEXT("Image Files\0*.png;*.jpeg;*.jpg\0");
        ofn.nFilterIndex = 1;
        ofn.lpstrFileTitle = NULL;
        ofn.nMaxFileTitle = 0;
        ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

        if (GetOpenFileName(&ofn) == TRUE)
        {
            //MessageBox(NULL, ofn.lpstrFile, TEXT("Selected File"), MB_OK);

            std::wstring wstr(ofn.lpstrFile);
            std::string filePath(wstr.begin(), wstr.end());

            // Restoring the original path
            std::filesystem::current_path(initialDir);

            return filePath;
        }

        // Restoring the original path
        std::filesystem::current_path(initialDir);

        return std::string("");
    }
}
