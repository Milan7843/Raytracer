#include "WindowUtility.h"

namespace WindowUtility
{
    namespace
    {
        GLFWwindow* savedWindow;
    }

    void setWindow(GLFWwindow* window)
    {
        savedWindow = window;
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
