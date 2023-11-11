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

    const ImVec4 BLACK_COL = ImVec4(0.09f, 0.09f, 0.09f, 1.0f);
    const ImVec4 DARK_GREY_COL = BLACK_COL + ImVec4(0.06f, 0.06f, 0.06f, 0.0f);
    const ImVec4 GREY_COL = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);
    const ImVec4 LIGHT_GREY_COL = ImVec4(0.27f, 0.27f, 0.27f, 1.0f);
    const ImVec4 WHITE_COL = ImVec4(0.4f, 0.4f, 0.4f, 1.0f);

    const ImVec4 RED_COL = ImVec4(0.7f, 0.08f, 0.14f, 1.0f);
    const ImVec4 GREEN_COL = ImVec4(0.0f, 0.58f, 0.0f, 1.0f);
    const ImVec4 YELLOW_COL = ImVec4(0.78f, 0.75f, 0.25f, 1.0f);
    const ImVec4 BLUE_COL = ImVec4(0.0f, 0.27f, 0.8f, 1.0f);
    const ImVec4 MAGENTA_COL = ImVec4(0.25f, 0.1f, 0.35f, 1.0f);
    const ImVec4 ORANGE_COL = ImVec4(0.95f, 0.4f, 0.12f, 1.0f);

    const ImVec4 LIGHT_MAGENTA_COL = MAGENTA_COL + LIGHTEN;
    const ImVec4 LIGHT_ORANGE_COL = ORANGE_COL + LIGHTEN;
    const ImVec4 LIGHT_RED_COL = RED_COL + LIGHTEN;
    const ImVec4 LIGHT_GREEN_COL = GREEN_COL + LIGHTEN;
    const ImVec4 LIGHT_YELLOW_COL = YELLOW_COL + LIGHTEN;
    const ImVec4 LIGHT_BLUE_COL = BLUE_COL + LIGHTEN;

    void SetOneDarkTheme();

    struct ScopedStyle {
        int style_count{0};

        ~ScopedStyle() {
            ImGui::PopStyleVar(style_count);
        }

        void push(ImGuiStyleVar idx, float val)  {
            ImGui::PushStyleVar(idx, val);
            style_count++;
        }

        void push(ImGuiStyleVar idx, const ImVec2 &val) {
            ImGui::PushStyleVar(idx, val);
            style_count++;
        }
    };

    void init_imgui(const Window &window);
    void destroy();
    void begin_frame();
    void end_frame();

    void Image(const Texture &texture, const ImVec2& size, const ImVec2& uv0 = ImVec2(0, 0), const ImVec2& uv1 = ImVec2(1, 1), const ImVec4& tint_col = ImVec4(1, 1, 1, 1), const ImVec4& border_col = ImVec4(0, 0, 0, 0));
}

#endif //TICHU_IMGUI_BUILD_H
