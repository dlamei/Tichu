# --- sockpp ---

add_library(sockpp STATIC)

target_sources(sockpp PRIVATE
		sockpp/src/acceptor.cpp
		sockpp/src/connector.cpp
		sockpp/src/datagram_socket.cpp
		sockpp/src/exception.cpp
		sockpp/src/inet_address.cpp
		sockpp/src/inet6_address.cpp
		sockpp/src/result.cpp
		sockpp/src/socket.cpp
		sockpp/src/stream_socket.cpp
)

set_target_properties(sockpp
		PROPERTIES
		POSITION_INDEPENDENT_CODE 1
		CXX_EXTENSIONS OFF
)

if (UNIX)
	target_sources(sockpp PRIVATE sockpp/src/unix/unix_address.cpp)
endif()

target_include_directories(sockpp
		PUBLIC
		sockpp/include
		sockpp/src
)

# --- rapidjson ---

add_library(rapidjson INTERFACE)
target_include_directories(rapidjson INTERFACE rapidjson/include)
target_compile_definitions(rapidjson INTERFACE RAPIDJSON_HAS_STD_STRING=1)

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


# --- group libraries by client / server ---

add_library(CLIENT_LIBS INTERFACE)
target_compile_definitions(CLIENT_LIBS INTERFACE TICHU_CLIENT=1)
target_link_libraries(CLIENT_LIBS INTERFACE
		glad
		glfw
		imgui
		glm
		stb
		rapidjson
		spdlog
		sockpp
)

add_library(SERVER_LIBS INTERFACE)
target_compile_definitions(SERVER_LIBS INTERFACE TICHU_SERVER=1)
target_link_libraries(SERVER_LIBS INTERFACE
		spdlog
		rapidjson
		sockpp
)

if (WIN32)
	target_link_libraries(CLIENT_LIBS INTERFACE wsock32 ws2_32)
	target_link_libraries(SERVER_LIBS INTERFACE wsock32 ws2_32)
endif()
