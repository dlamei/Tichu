#include "Tichu.h"

#include "../GUI/imgui_build.h"
#include <imgui.h>
#include <imgui_stdlib.h>

//bool Tichu::OnInit()
//{
//    // Allow loading of JPEG  and PNG image files
//    wxImage::AddHandler(new wxJPEGHandler());
//    wxImage::AddHandler(new wxPNGHandler());
//
//    // Open main game window
//    GameWindow* gameWindow = new GameWindow(
//            "TICHU", // title of window,
//            wxDefaultPosition, // position of the window
//            wxDefaultSize // size of the window
//    );
//    gameWindow->Show(true);
//
//    // Initialize game controller
//    GameController::init(gameWindow);
//
//    return true;
//}

void show_input_label(const char *label) {
    ImGui::TableNextRow();
    ImGui::TableNextColumn();
    ImGui::Text("%s", label);
    ImGui::TableNextColumn();
}

void team_selectable(const char *label, TeamSelection *selection, TeamSelection team, float height) {
    ImGui::TableNextColumn();
    if (ImGui::RoundedSelectable(label, team == *selection, 0, {0, height})) {
        *selection = team;
    }
}

void show_connection_panel(ConnectionPanelInput *input) {

    auto height = ImGui::GetFontSize();

    auto style = ImGui::ScopedStyle{};
    style.push(ImGuiStyleVar_WindowMinSize, {750, height * 18});
    style.push(ImGuiStyleVar_CellPadding, {height / 2, height / 2});
    style.push(ImGuiStyleVar_WindowPadding, {height, height});
    style.push(ImGuiStyleVar_SelectableTextAlign, {0.5, 0.5});

    ImGui::Begin("Connection", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoDocking);
    ImGui::SetWindowFontScale(1.3);

    ImGui::BeginTable("input fields", 2);
    ImGui::TableSetupColumn("field name", ImGuiTableColumnFlags_WidthFixed) ;
    ImGui::TableSetupColumn("field input", ImGuiTableColumnFlags_WidthStretch) ;

    show_input_label("SERVER ADDRESS    ");
    ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
    ImGui::InputText("##address", &input->host);

    show_input_label("SERVER PORT    ");
    ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
    ImGui::InputScalar("##port", ImGuiDataType_U32, &input->port);

    show_input_label("PLAYER NAME    ");
    ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
    ImGui::InputText("##name", &input->name);

    ImGui::EndTable();

    ImGui::BeginTable("buttons", 3);
    team_selectable("TEAM 1", &input->team, TeamSelection::TEAM_1, height * 2);
    team_selectable("TEAM 2", &input->team, TeamSelection::TEAM_2, height * 2);
    team_selectable("RANDOM", &input->team, TeamSelection::RANDOM, height * 2);

    ImGui::TableNextRow();
    ImGui::TableNextRow();
    ImGui::TableNextColumn();
    ImGui::TableNextColumn();
    ImGui::TableNextColumn();
    if (ImGui::ButtonEx("CONNECT", {ImGui::GetContentRegionAvail().x, height * 2})) {
        input->connect = true;
    }
    ImGui::EndTable();

    ImGui::End();
}
