
//
// setup imgui with some new defaults
//


#ifndef TICHU_IMGUI_BUILD_H
#define TICHU_IMGUI_BUILD_H

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#include <imgui_internal.h>
#include "window.h"
#include "gl_utils.h"

namespace ImGui {

    const float FONT_HEIGHT = 25.0f;

    const ImVec4 LIGHTEN = ImVec4(0.2f, 0.2f, 0.2f, 0.0f);

    const ImVec4 BLACK = ImVec4(0.09f, 0.09f, 0.09f, 1.0f);
    const ImVec4 DARK_GREY = BLACK + ImVec4(0.06f, 0.06f, 0.06f, 0.0f);
    const ImVec4 GREY = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);
    const ImVec4 LIGHT_GREY = ImVec4(0.27f, 0.27f, 0.27f, 1.0f);
    const ImVec4 WHITE = ImVec4(0.4f, 0.4f, 0.4f, 1.0f);

    const ImVec4 RED = ImVec4(0.7f, 0.08f, 0.14f, 1.0f);
    const ImVec4 GREEN = ImVec4(0.0f, 0.58f, 0.0f, 1.0f);
    const ImVec4 YELLOW = ImVec4(0.78f, 0.75f, 0.25f, 1.0f);
    const ImVec4 BLUE = ImVec4(0.0f, 0.27f, 0.8f, 1.0f);
    const ImVec4 MAGENTA = ImVec4(0.25f, 0.1f, 0.35f, 1.0f);
    const ImVec4 ORANGE = ImVec4(0.95f, 0.4f, 0.12f, 1.0f);

    const ImVec4 LIGHT_MAGENTA = MAGENTA + LIGHTEN;
    const ImVec4 LIGHT_ORANGE = ORANGE + LIGHTEN;
    const ImVec4 LIGHT_RED = RED + LIGHTEN;
    const ImVec4 LIGHT_GREEN = GREEN + LIGHTEN;
    const ImVec4 LIGHT_YELLOW = YELLOW + LIGHTEN;
    const ImVec4 LIGHT_BLUE = BLUE + LIGHTEN;

    void SetOneDarkTheme();

    // helper struct for defining a style in a scope. these styles will automatically get reset when the struct goes out of scope
    struct ScopedStyle {
        int style_count{0};
        int col_count{0};

        ImGuiStyleVar style;

        ~ScopedStyle() {
            ImGui::PopStyleColor(col_count);
            ImGui::PopStyleVar(style_count);
        }

        void push_style(ImGuiStyleVar idx, float val)  {
            ImGui::PushStyleVar(idx, val);
            style_count++;
        }

        void push_style(ImGuiStyleVar idx, const ImVec2 &val) {
            ImGui::PushStyleVar(idx, val);
            style_count++;
        }

        void push_color(ImGuiCol idx, const ImVec4 &color) {
            ImGui::PushStyleColor(idx, color);
            col_count++;
        }
    };

    // initialize imgui
    void init_imgui(const Window &window);

    // destroy imgui context
    void destroy();

    // should be called at the start of every frame
    void begin_frame();

    // should be called at the end of every frame
    void end_frame();

    void Image(const Texture &texture, const ImVec2& size, const ImVec2& uv0 = ImVec2(0, 0), const ImVec2& uv1 = ImVec2(1, 1), const ImVec4& tint_col = ImVec4(1, 1, 1, 1), const ImVec4& border_col = ImVec4(0, 0, 0, 0));
}

#endif //TICHU_IMGUI_BUILD_H
