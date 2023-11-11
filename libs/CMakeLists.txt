# --- sockpp ---

add_subdirectory(sockpp)
include_directories(sockpp/include)

# --- rapidjson ---

add_library(rapidjson INTERFACE)
target_include_directories(rapidjson INTERFACE rapidjson/include)

# --- glfw ---

add_subdirectory(glfw)
include_directories(glfw/include)

# --- glm ---

add_subdirectory(glm)
include_directories(glm)

# --- spdlog ---

add_subdirectory(spdlog)
include_directories(spdlog/include)

# --- stb ---

add_library(stb STATIC)
target_sources(stb PRIVATE stb/stb_image.cpp)
target_include_directories(stb PUBLIC stb)

# --- glad ---

add_library(glad STATIC)
target_sources(glad PRIVATE glad/src/glad.c)
target_include_directories(glad PUBLIC glad/include)

# --- imgui ---

add_library(imgui STATIC)
target_sources(imgui
		PRIVATE
		imgui/imgui_demo.cpp
		imgui/imgui_draw.cpp
		imgui/imgui_tables.cpp
		imgui/imgui_widgets.cpp
		imgui/imgui.cpp

		imgui/backends/imgui_impl_opengl3.cpp
		imgui/backends/imgui_impl_glfw.cpp

		imgui/misc/cpp/imgui_stdlib.cpp
)

target_include_directories( imgui
		PUBLIC
		imgui
		imgui/backends
		imgui/misc/cpp
)

