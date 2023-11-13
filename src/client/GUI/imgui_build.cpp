#include "imgui_build.h"

#include "window.h"

#include <imgui_impl_opengl3.h>
#include <imgui_impl_glfw.h>
#include <GLFW/glfw3.h>

// embed font in executable
const uint8_t c_RobotRegular[] = {
#include "font.embed"
};

namespace ImGui {

    void SetOneDarkTheme() {
        const auto BACKGROUND = BLACK;

        const auto ACCENT = RED;
        const auto ACCENT_HOVER = ORANGE_COL;

        ImGui::StyleColorsDark();

        ImGuiStyle &style = ImGui::GetStyle();

        style.WindowBorderSize = 0;
        style.PopupBorderSize = 0;
        style.FramePadding = {10, 10};
        style.FrameRounding = 10;
        style.WindowRounding = 10;
        style.TabRounding = 12;
        style.GrabRounding = 2;
        style.DockingSeparatorSize = 5;

        auto &colors = style.Colors;
        colors[ImGuiCol_WindowBg] = BACKGROUND;

        // Headers
        colors[ImGuiCol_Header] = DARK_GREY;
        colors[ImGuiCol_HeaderHovered] = GREY;
        colors[ImGuiCol_HeaderActive] = LIGHT_GREY;
        colors[ImGuiCol_MenuBarBg] = BACKGROUND;

        // Buttons
        colors[ImGuiCol_Button] = BACKGROUND;
        colors[ImGuiCol_ButtonHovered] = LIGHT_GREY;
        colors[ImGuiCol_ButtonActive] = GREY;

        // Frame BG
        colors[ImGuiCol_FrameBg] = DARK_GREY;
        colors[ImGuiCol_FrameBgHovered] = LIGHT_GREY;
        colors[ImGuiCol_FrameBgActive] = LIGHT_GREY;

        // Seperator
        colors[ImGuiCol_SeparatorHovered] = ACCENT;
        colors[ImGuiCol_SeparatorActive] = ACCENT_HOVER;
        colors[ImGuiCol_ResizeGrip] = GREY;
        colors[ImGuiCol_ResizeGripHovered] = ACCENT;
        colors[ImGuiCol_ResizeGripActive] = ACCENT_HOVER;
        colors[ImGuiCol_Border] = BACKGROUND;

        // Plot
        colors[ImGuiCol_PlotHistogram] = ACCENT;
        colors[ImGuiCol_PlotHistogramHovered] = ACCENT_HOVER;

        // Docking
        colors[ImGuiCol_DockingPreview] = ACCENT;
        colors[ImGuiCol_DockingEmptyBg] = DARK_GREY;

        // NavWindow
        colors[ImGuiCol_NavWindowingHighlight] = ACCENT;

        //Checkmarks
        colors[ImGuiCol_CheckMark] = ACCENT;
        colors[ImGuiCol_SliderGrab] = ACCENT;
        colors[ImGuiCol_SliderGrabActive] = ACCENT_HOVER;
        colors[ImGuiCol_DragDropTarget] = ACCENT;

        // Tabs
        colors[ImGuiCol_Tab] = LIGHT_GREY;
        colors[ImGuiCol_TabHovered] = DARK_GREY;
        colors[ImGuiCol_TabActive] = BACKGROUND;
        colors[ImGuiCol_TabUnfocused] = GREY;
        colors[ImGuiCol_TabUnfocusedActive] = BACKGROUND;

        // Title
        colors[ImGuiCol_TitleBg] = GREY;
        colors[ImGuiCol_TitleBgActive] = LIGHT_GREY;
        colors[ImGuiCol_TitleBgCollapsed] = DARK_GREY;

        // Navigation
        colors[ImGuiCol_TextSelectedBg] = LIGHT_GREY;
        colors[ImGuiCol_NavHighlight] = ACCENT;
    }


    void init_imgui(const Window &window) {

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO &io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        io.ConfigDockingWithShift = true;
        //io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

        io.IniFilename = nullptr;
        io.LogFilename = nullptr;

        ImGuiStyle &style = ImGui::GetStyle();
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
            style.WindowRounding = 0.0f;
            style.Colors[ImGuiCol_WindowBg].w = 1.0f;
        }

        ImGui_ImplGlfw_InitForOpenGL(window.get_native_window(), true);
        ImGui_ImplOpenGL3_Init();

        {
            ImFontConfig fontConfig;
            fontConfig.FontDataOwnedByAtlas = false;
            ImFont *robotFont = io.Fonts->AddFontFromMemoryTTF(
                    (void *) c_RobotRegular, sizeof(c_RobotRegular), FONT_HEIGHT, &fontConfig);
            io.FontDefault = robotFont;
            ImGui_ImplOpenGL3_CreateFontsTexture();
        }

        ImGui::SetOneDarkTheme();
    }

    void destroy() {
        ImGui::DestroyContext();
    }

    void begin_frame() {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        //ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());
    }

    void end_frame() {
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        ImGuiIO &io = ImGui::GetIO();


        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
            GLFWwindow *backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);
        }
    }

    void Image(const Texture &texture, const ImVec2 &size, const ImVec2 &uv0, const ImVec2 &uv1,
                      const ImVec4 &tint_col, const ImVec4 &border_col) {
        ImGui::Image((void *)(size_t)texture.native_texture(), size, uv0, uv1, tint_col, border_col);
    }

}