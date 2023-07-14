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

class Scene;

class ContextMenuSource
{
public:
    virtual void renderContextMenu(Scene& scene) final
    {
        if (ImGui::BeginPopup(id))
        {
            // Rendering the context menu specific to the subclass
            renderContextMenuItems(scene);

            ImGui::EndPopup();
        }
        if (openNextCall)
        {
            ImGui::OpenPopup(id);
            openNextCall = false;
        }
    }

    virtual void openContextMenu() final
    {
        openNextCall = true;
    }

protected:
	ContextMenuSource()
    {
        std::string idStr = std::to_string(instanceCount++);
        id = idStr.c_str();
    }
	~ContextMenuSource() {
    }

    virtual void renderContextMenuItems(Scene& scene) = 0;

private:

    bool openNextCall{ false };

    const char* id;

    // Static variable to keep track of the instance count to give each instance a unique ID
    static unsigned int instanceCount;
};

