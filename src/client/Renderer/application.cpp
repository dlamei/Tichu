#include "application.h"
#include "window.h"
#include "renderer.h"
#include "imgui_build.h"

#include "../../common/utils.h"

Application *Application::s_instance = nullptr;

Application::Application(const ApplicationCreateInfo &info)
    : _info(info)
{
    init_logger();
    ASSERT(!s_instance, "Application already created!");

    s_instance = this;

    WindowCreateInfo win_info{
            .title = info.title,
            .size = info.size,
            .position = info.position,
            .vsync = true,
    };

    _window = std::make_unique<Window>(win_info);

    Renderer::init();
}

void Application::run() {
    _last_frame_time = (float) _window->get_time();

    while (!_window->should_close()) {
        _window->on_update();
        update();
    }
}

void Application::update_frame() {
    ASSERT(s_instance, "application was not initialized");
    s_instance->update();
}

void Application::update() {

    auto time = (float) _window->get_time();
    auto time_step = TimeStep(time - _last_frame_time);
    _last_frame_time = time;

    ImGui::begin_frame();
    build_dock_frame();

    Renderer::resize_frame_buffer();
    Renderer::bind_frame_buffer();

    for (auto &layer: _layers) {
        layer->on_update(time_step);
        layer->on_imgui();
    }

    Renderer::flush();
    Renderer::unbind_frame_buffer();

    ImGui::end_frame();

    _frame_count += 1;
}

Application::~Application() {
    for (auto &layer: _layers) {
        layer->on_detach();
    }

    _window->destroy();
}

// creates a viewport window that is always in the background, called viewport
void Application::build_dock_frame() {

    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking;

    ImGuiViewport *viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->Pos);
    ImGui::SetNextWindowSize(viewport->Size);
    ImGui::SetNextWindowViewport(viewport->ID);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
                    ImGuiWindowFlags_NoMove;
    window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("DockSpace", nullptr, window_flags);
    ImGui::PopStyleVar(3);

    ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
    ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f));

    if (_frame_count == 0) {

        ImGui::DockBuilderRemoveNode(dockspace_id); // clear any previous layout
        ImGui::DockBuilderAddNode(dockspace_id, ImGuiDockNodeFlags_DockSpace);
        ImGui::DockBuilderSetNodeSize(dockspace_id, viewport->Size);

        auto viewport_dock = ImGui::DockBuilderAddNode(dockspace_id, ImGuiDockNodeFlags_HiddenTabBar |
                                                                 ImGuiDockNodeFlags_NoDockingOverMe);
        ImGuiID stats_dock;
        ImGui::DockBuilderSplitNode(viewport_dock, ImGuiDir_Right, 0.25f, &stats_dock, &viewport_dock);
        ImGui::DockBuilderGetNode(viewport_dock)->LocalFlags |= ImGuiDockNodeFlags_NoTabBar;
        ImGui::DockBuilderGetNode(viewport_dock)->LocalFlags |= ImGuiDockNodeFlags_NoDockingOverCentralNode;
        ImGui::DockBuilderGetNode(stats_dock)->LocalFlags |= ImGuiDockNodeFlags_NoWindowMenuButton;

        ImGui::DockBuilderDockWindow("viewport", viewport_dock);
        ImGui::DockBuilderDockWindow("###GameInfo", stats_dock);

        ImGui::DockBuilderFinish(dockspace_id);
    }

    ImGui::End();

    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImGui::DARK_GREY);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, {0, 0});
    ImGui::Begin("viewport");
    _viewport_size = {ImGui::GetWindowWidth(), ImGui::GetWindowHeight()};
    _viewport_pos = {ImGui::GetWindowPos().x, ImGui::GetWindowPos().y};
    ImGui::End();
    ImGui::PopStyleVar();
    ImGui::PopStyleColor();

    auto win_size = _window->get_window_size();
    _window_size = {win_size.first, win_size.second};

    //auto win_pos = _window->get_mouse_pos();
    //_window_mouse_pos = {win_pos.first, win_pos.second};
}

const Application *Application::get_instance() {
    ASSERT(s_instance, "Application was not initialized");
    return s_instance;
}

void Application::push_layer(const std::shared_ptr<Layer> &layer) {
    layer->on_attach();
    _layers.push_back(layer);
}

glm::vec2 Application::get_mouse_pos() {
    auto pos = get_instance()->_window->get_mouse_pos();
    return {pos.first, pos.second};
}

