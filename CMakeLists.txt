cmake_minimum_required(VERSION 3.15)
project(Tichu) # your project name
set(CMAKE_CXX_STANDARD 20)

set(COMMON_SOURCE_FILES
		src/common/game_state/cards/card.cpp src/common/game_state/cards/card.h
		src/common/game_state/game_state.cpp src/common/game_state/game_state.h
		src/common/game_state/player/hand.cpp src/common/game_state/player/hand.h
		src/common/game_state/player/player.cpp src/common/game_state/player/player.h
		src/common/game_state/cards/draw_pile.cpp src/common/game_state/cards/draw_pile.h
		src/common/game_state/cards/discard_pile.cpp src/common/game_state/cards/discard_pile.h
		src/common/network/client_msg.cpp src/common/network/client_msg.h
		src/common/network/server_msg.cpp src/common/network/server_msg.h
		src/common/serialization/serializable.h
		src/common/serialization/serializable.cpp
		src/common/logging.h
)

set(CLIENT_SOURCE_FILES
		src/client/main.cpp
		src/client/GUI/window.cpp
		src/client/GUI/window.h
		src/client/GUI/gl_utils.cpp
		src/client/GUI/gl_utils.h
		src/client/GUI/Application.cpp
		src/client/GUI/Application.h
		src/client/GUI/renderer.cpp
		src/client/GUI/renderer.h
		src/client/GUI/imgui_build.cpp
		src/client/GUI/imgui_build.h
        src/client/Tichu.h
        src/client/Tichu.cpp
)

set(SERVER_SOURCE_FILES
		src/server/main.cpp
		src/server/game_instance.cpp src/server/game_instance.h
		src/server/game_instance_manager.cpp src/server/game_instance_manager.h
		src/server/player_manager.cpp src/server/player_manager.h
		src/server/server_network_manager.cpp src/server/server_network_manager.h
		src/server/request_handler.h src/server/request_handler.cpp
)


add_executable(Tichu-client ${CLIENT_SOURCE_FILES} ${COMMON_SOURCE_FILES})
add_executable(Tichu-server ${SERVER_SOURCE_FILES} ${COMMON_SOURCE_FILES})

target_compile_definitions(Tichu-client PRIVATE TICHU_CLIENT=1 RAPIDJSON_HAS_STDSTRING=1)
target_compile_definitions(Tichu-client PRIVATE PRINT_NETWORK_MESSAGES=1)

target_compile_definitions(Tichu-server PRIVATE TICHU_SERVER=1 RAPIDJSON_HAS_STDSTRING=1)
target_compile_definitions(Tichu-server PRIVATE PRINT_NETWORK_MESSAGES=1)

add_subdirectory(libs)

target_link_libraries(Tichu-client
		glad
		glfw
		spdlog
		imgui
		glm
		stb
		rapidjson
)

target_link_libraries(Tichu-server
		spdlog
		rapidjson
)

# linking to sockpp
if(WIN32)
	target_link_libraries(Tichu-client sockpp)
	target_link_libraries(Tichu-server sockpp)

	target_link_libraries(Tichu-client wsock32 ws2_32)
else()

	set(THREADS_PREFER_PTHREAD_FLAG ON)
	find_package(Threads REQUIRED)

	target_link_libraries(Tichu-client sockpp Threads::Threads)
	target_link_libraries(Tichu-server sockpp Threads::Threads)

endif()

# copy assets (images) to binary directory
file(INSTALL assets DESTINATION ${CMAKE_BINARY_DIR})


set(CMAKE_CXX_FLAGS "--coverage")
add_library(Tichu-lib ${SERVER_SOURCE_FILES} ${CLIENT_SOURCE_FILES} ${COMMON_SOURCE_FILES})
target_compile_definitions(Tichu-lib PRIVATE TICHU_CLIENT=1 TICHU_SERVER=1 RAPIDJSON_HAS_STDSTRING=1)

add_subdirectory(googletest)
add_subdirectory(unit-tests)
