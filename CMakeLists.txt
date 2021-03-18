cmake_minimum_required(VERSION 3.15)
project(Lama) # your project name

set(CMAKE_CXX_STANDARD 20)

include_directories(sockpp/include)

find_package(wxWidgets COMPONENTS core base net REQUIRED)
include(${wxWidgets_USE_FILE})

# define a variable CLIENT_SOURCE_FILES that contains the paths to all source files required to compile the client executable
set(CLIENT_SOURCE_FILES
        src/client/main.cpp
        src/client/app/Lama.cpp src/client/app/Lama.h
        src/client/GameController.cpp src/client/GameController.h
        # UI
        src/client/windows/GameWindow.cpp src/client/windows/GameWindow.h
        src/client/uiElements/ImagePanel.cpp src/client/uiElements/ImagePanel.h
        src/client/panels/ConnectionPanel.cpp src/client/panels/ConnectionPanel.h
        src/client/panels/MainGamePanel.cpp src/client/panels/MainGamePanel.h
        src/client/uiElements/InputField.cpp src/client/uiElements/InputField.h
        src/client/uiElements/ImagePanel.cpp src/client/uiElements/ImagePanel.h
        # network
        src/client/network/ClientNetworkManager.cpp src/client/network/ClientNetworkManager.h
        src/client/network/ResponseListenerThread.cpp src/client/network/ResponseListenerThread.h
        # game state
        src/game_state/cards/card.cpp src/game_state/cards/card.h
        src/game_state/game_state.cpp src/game_state/game_state.h
        src/game_state/player/hand.cpp src/game_state/player/hand.h
        src/game_state/player/player.cpp src/game_state/player/player.h
        src/game_state/cards/draw_pile.cpp src/game_state/cards/draw_pile.h
        src/game_state/cards/discard_pile.cpp src/game_state/cards/discard_pile.h
        # client requests
        src/network/requests/client_request.cpp src/network/requests/client_request.h
        src/network/requests/play_card_request.cpp src/network/requests/play_card_request.h
        src/network/requests/draw_card_request.cpp src/network/requests/draw_card_request.h
        src/network/requests/fold_request.cpp src/network/requests/fold_request.h
        src/network/requests/join_game_request.cpp src/network/requests/join_game_request.h
        src/network/requests/start_game_request.cpp src/network/requests/start_game_request.h
        # server responses
        src/network/responses/server_response.cpp src/network/responses/server_response.h
        src/network/responses/request_response.cpp src/network/responses/request_response.h
        src/network/responses/full_state_response.cpp src/network/responses/full_state_response.h
        # utils
        src/common/utils/json_utils.h
        src/common/utils/uuid_generator.h
        # reactive state
        src/reactive_state/serializable.h
        src/reactive_state/array_helpers.h
        src/reactive_state/value_type_helpers.h
        src/reactive_state/vector_utils.h
        src/reactive_state/reactive_objects/reactive_value.h
        src/reactive_state/reactive_objects/reactive_object.cpp src/reactive_state/reactive_objects/reactive_object.h src/client/network/ClientNetworkManager.cpp src/client/network/ClientNetworkManager.h)

# define a variable SERVER_SOURCE_FILES that contains the paths to all source files required to compile the server executable
set(SERVER_SOURCE_FILES
        src/server/main.cpp
        src/server/game_instance.cpp src/server/game_instance.h
        src/server/game_instance_manager.cpp src/server/game_instance_manager.h
        src/server/player_manager.cpp src/server/player_manager.h
        src/server/server_network_manager.cpp src/server/server_network_manager.h
        # game state
        src/game_state/cards/card.cpp src/game_state/cards/card.h
        src/game_state/game_state.cpp src/game_state/game_state.h
        src/game_state/player/hand.cpp src/game_state/player/hand.h
        src/game_state/player/player.cpp src/game_state/player/player.h
        src/game_state/cards/draw_pile.cpp src/game_state/cards/draw_pile.h
        src/game_state/cards/discard_pile.cpp src/game_state/cards/discard_pile.h
        # client requests
        src/network/requests/client_request.cpp src/network/requests/client_request.h
        src/network/requests/play_card_request.cpp src/network/requests/play_card_request.h
        src/network/requests/draw_card_request.cpp src/network/requests/draw_card_request.h
        src/network/requests/fold_request.cpp src/network/requests/fold_request.h
        src/network/requests/join_game_request.cpp src/network/requests/join_game_request.h
        src/network/requests/start_game_request.cpp src/network/requests/start_game_request.h
        # server responses
        src/network/responses/server_response.cpp src/network/responses/server_response.h
        src/network/responses/request_response.cpp src/network/responses/request_response.h
        src/network/responses/full_state_response.cpp src/network/responses/full_state_response.h
        # utils
        src/common/utils/json_utils.h
        src/common/utils/uuid_generator.h
        # reactive state
        src/reactive_state/serializable.h
        src/reactive_state/array_helpers.h
        src/reactive_state/value_type_helpers.h
        src/reactive_state/vector_utils.h
        src/reactive_state/reactive_objects/reactive_value.h
        src/reactive_state/reactive_objects/reactive_object.cpp src/reactive_state/reactive_objects/reactive_object.h)


# set source files for client-executable
add_executable(Lama-client ${CLIENT_SOURCE_FILES})
# set compile directives for client-executable
target_compile_definitions(Lama-client PRIVATE LAMA_CLIENT=1 RAPIDJSON_HAS_STDSTRING=1)
# link with wxWidgets
target_link_libraries(Lama-client ${wxWidgets_LIBRARIES})


# set source files for server-executable
add_executable(Lama-server ${SERVER_SOURCE_FILES})
# set compile directives for server-executable
target_compile_definitions(Lama-server PRIVATE LAMA_SERVER=1 RAPIDJSON_HAS_STDSTRING=1)
# Comment out if you don't want to print messages received into the console
target_compile_definitions(Lama-server PRIVATE PRINT_NETWORK_MESSAGES=1)


# linking to sockpp
if(WIN32)
    message("Detected Win32")

    target_link_libraries(Lama-client ${CMAKE_SOURCE_DIR}/sockpp/cmake-build-debug/sockpp-static.lib)
    target_link_libraries(Lama-server ${CMAKE_SOURCE_DIR}/sockpp/cmake-build-debug/sockpp-static.lib)

    # Necessary to get sockets working under Windows (with MingW)
    target_link_libraries(Lama-client wsock32 ws2_32)
else()
    message("Not Win32, so probably a Linux") # We assume it's Linux in this case

    set(THREADS_PREFER_PTHREAD_FLAG ON)
    find_package(Threads REQUIRED)

    target_link_libraries(Lama-client ${CMAKE_SOURCE_DIR}/sockpp/cmake-build-debug/libsockpp.so Threads::Threads)
    target_link_libraries(Lama-server ${CMAKE_SOURCE_DIR}/sockpp/cmake-build-debug/libsockpp.so Threads::Threads)
endif()

# copy assets (images) to binary directory
file(INSTALL assets DESTINATION ${CMAKE_BINARY_DIR})