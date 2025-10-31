#pragma once
#include "imgui.h" // IMGUI_IMPL_API
// Follow "Getting Started" link and check examples/ folder to learn about using backends!
IMGUI_IMPL_API bool ImGui_ImplOpenGL3_Init(const char *glsl_version = nullptr);
IMGUI_IMPL_API void ImGui_ImplOpenGL3_Shutdown();
IMGUI_IMPL_API void ImGui_ImplOpenGL3_NewFrame();
IMGUI_IMPL_API void ImGui_ImplOpenGL3_RenderDrawData(ImDrawData *draw_data);

// (Optional) Called by Init/NewFrame/Shutdown
IMGUI_IMPL_API bool ImGui_ImplOpenGL3_CreateDeviceObjects();
IMGUI_IMPL_API void ImGui_ImplOpenGL3_DestroyDeviceObjects();

// (Advanced) Use e.g. if you need to precisely control the timing of texture updates (e.g. for staged rendering), by setting ImDrawData::Textures = NULL to handle this manually.
IMGUI_IMPL_API void ImGui_ImplOpenGL3_UpdateTexture(ImTextureData *tex);
