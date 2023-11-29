cmake_minimum_required(VERSION 3.15)
project(Tichu)
set(CMAKE_CXX_STANDARD 20)

if (WIN32)
	add_compile_options(/EHsc)
endif()

add_subdirectory(libs)

set(COMMON_SOURCE_FILES
        src/common/game_state/cards/card.cpp src/common/game_state/cards/card.h
        src/common/game_state/GameState.cpp src/common/game_state/GameState.h
        src/common/game_state/player/hand.cpp src/common/game_state/player/hand.h
		src/common/game_state/player/Player.cpp src/common/game_state/player/Player.h
		src/common/game_state/cards/WonCardsPile.cpp src/common/game_state/cards/WonCardsPile.h
        src/common/game_state/cards/DrawPile.cpp src/common/game_state/cards/DrawPile.h
        src/common/game_state/cards/ActivePile.cpp src/common/game_state/cards/ActivePile.h
        src/common/game_state/cards/CardCombination.cpp src/common/game_state/cards/CardCombination.h
		src/common/utils.cpp
		src/common/Messages.h
)

set(CLIENT_SOURCE_FILES
		src/client/Renderer/window.cpp
		src/client/Renderer/window.h
		src/client/Renderer/gl_utils.cpp
		src/client/Renderer/gl_utils.h
		src/client/Renderer/Application.cpp
		src/client/Renderer/Application.h
		src/client/Renderer/renderer.cpp
		src/client/Renderer/renderer.h
		src/client/Renderer/imgui_build.cpp
		src/client/Renderer/imgui_build.h
        src/client/Tichu.h
        src/client/Tichu.cpp
		src/client/panels.cpp
		src/client/panels.h
        src/client/GamePanel.cpp
        src/client/GamePanel.h
		src/client/Renderer/KeyCodes.h
)

set(SERVER_SOURCE_FILES
        src/server/GameInstance.cpp src/server/GameInstance.h
		src/server/game_instance_manager.cpp src/server/game_instance_manager.h
		src/server/player_manager.cpp src/server/player_manager.h
		src/server/server_network_manager.cpp src/server/server_network_manager.h
		src/server/request_handler.h src/server/request_handler.cpp
)

# --- copy assets into build directory ---
file(COPY assets DESTINATION ${CMAKE_BINARY_DIR})

# --- client / server targets ---
add_executable(Tichu-client ${CLIENT_SOURCE_FILES} ${COMMON_SOURCE_FILES} src/client/main.cpp)
add_executable(Tichu-server ${SERVER_SOURCE_FILES} ${COMMON_SOURCE_FILES} src/server/main.cpp)

target_link_libraries(Tichu-client CLIENT_LIBS)
target_link_libraries(Tichu-server SERVER_LIBS)


# --- tests ---
set(CMAKE_CXX_FLAGS "--coverage")
add_library(Tichu-lib ${SERVER_SOURCE_FILES} ${CLIENT_SOURCE_FILES} ${COMMON_SOURCE_FILES})
target_link_libraries(Tichu-lib CLIENT_LIBS SERVER_LIBS)

add_subdirectory(googletest)
add_subdirectory(unit-tests)