cmake_minimum_required(VERSION 3.15)
project(Tichu)
set(CMAKE_CXX_STANDARD 20)

if (WIN32)
	add_compile_options(/EHsc)
endif()

add_subdirectory(libs)

set(COMMON_SOURCE_FILES
		src/common/event.cpp src/common/event.h
        src/common/game_state/cards/card.cpp src/common/game_state/cards/card.h
		src/common/game_state/game_state.cpp src/common/game_state/game_state.h
        src/common/game_state/player/hand.cpp src/common/game_state/player/hand.h
		src/common/game_state/player/player.cpp src/common/game_state/player/player.h
		src/common/game_state/cards/won_cards_pile.cpp src/common/game_state/cards/won_cards_pile.h
		src/common/game_state/cards/draw_pile.cpp src/common/game_state/cards/draw_pile.h
		src/common/game_state/cards/active_pile.cpp src/common/game_state/cards/active_pile.h
		src/common/game_state/cards/card_combination.cpp src/common/game_state/cards/card_combination.h
		src/common/utils.cpp
		src/common/messages.h
		src/common/listener.h
)

set(CLIENT_SOURCE_FILES
		src/client/Renderer/window.cpp
		src/client/Renderer/window.h
		src/client/Renderer/gl_utils.cpp
		src/client/Renderer/gl_utils.h
        src/client/Renderer/application.cpp
        src/client/Renderer/application.h
		src/client/Renderer/renderer.cpp
		src/client/Renderer/renderer.h
		src/client/Renderer/imgui_build.cpp
		src/client/Renderer/imgui_build.h
		src/client/tichu.h
		src/client/tichu.cpp
		src/client/panels.cpp
		src/client/panels.h
        src/client/game_panel.cpp
        src/client/game_panel.h
		src/client/Renderer/key_codes.h
)

set(SERVER_SOURCE_FILES
		src/server/game_instance.cpp src/server/game_instance.h
		src/server/game_instance_manager.cpp src/server/game_instance_manager.h
		src/server/player_manager.cpp src/server/player_manager.h
		src/server/server_network_manager.cpp src/server/server_network_manager.h
		src/server/request_handler.h src/server/request_handler.cpp
		src/server/server.cpp
		src/server/server.h
)

# --- copy assets into build directory ---
file(COPY assets DESTINATION ${CMAKE_BINARY_DIR})

# --- client / server targets ---
add_executable(Tichu-client ${CLIENT_SOURCE_FILES} ${COMMON_SOURCE_FILES} src/client/main.cpp)
add_executable(Tichu-server ${SERVER_SOURCE_FILES} ${COMMON_SOURCE_FILES} src/server/main.cpp)

target_link_libraries(Tichu-client CLIENT_LIBS)
target_link_libraries(Tichu-server SERVER_LIBS)


# --- tests ---
if (NOT MSVC)
	set(CMAKE_CXX_FLAGS "--coverage")
endif()
add_library(Tichu-lib ${SERVER_SOURCE_FILES} ${CLIENT_SOURCE_FILES} ${COMMON_SOURCE_FILES})
target_link_libraries(Tichu-lib CLIENT_LIBS SERVER_LIBS)

add_subdirectory(googletest)
add_subdirectory(unit-tests)
