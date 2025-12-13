#include "ProfileLayer.h"
#include "imgui.h"
namespace Engine::Profile
{

    std::vector<ProfileResult> ProfileLayer::ProfileResults = std::vector<ProfileResult>();

    ProfileLayer::ProfileLayer()
        : Layer("ProfileLayer")
    {
    }

    ProfileLayer::~ProfileLayer()
    {
    }

    void ProfileLayer::OnAttach()
    {
    }

    void ProfileLayer::OnDetach()
    {
    }

    void ProfileLayer::OnImGuiRender()
    {
        ENGINE_PROFILING_FUNC();
        ImGui::Begin("Profile Results");
        for (auto result : ProfileResults)
        {
            char buf[256];
            strcpy_s(buf, sizeof(buf), "%6.3f ms  ");
            strcat_s(buf, sizeof(buf), result.Name.c_str());
            ImGui::Text(buf, result.Time);
        }
        ProfileResults.clear();
        ImGui::End();
    }
} // namespace Engine::Profile