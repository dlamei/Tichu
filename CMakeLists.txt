cmake_minimum_required(VERSION 3.15)
project(Lama-client) # your project name

set(CMAKE_CXX_STANDARD 20)

include_directories(sockpp/include)

find_package(wxWidgets COMPONENTS core base net REQUIRED)
include(${wxWidgets_USE_FILE})



set(SOURCE_FILES
        src/client/main.cpp
        src/client/windows/GameWindow.cpp src/client/windows/GameWindow.h
        src/client/uiElements/ImagePanel.cpp src/client/uiElements/ImagePanel.h
        src/client/panels/ConnectionPanel.cpp src/client/panels/ConnectionPanel.h
        src/client/uiElements/InputField.cpp src/client/uiElements/InputField.h
        src/client/uiElements/ImagePanel.cpp src/client/uiElements/ImagePanel.h
        src/client/network/ClientNetworkThread.cpp src/client/network/ClientNetworkThread.h


        # src/game.cpp src/game.h   # not needed in client
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
        src/network/requests/leave_game_request.cpp src/network/requests/leave_game_request.h
        # server responses
        src/network/responses/server_response.cpp src/network/responses/server_response.h
        src/network/responses/request_response.cpp src/network/responses/request_response.h
        src/network/responses/state_diff_response.cpp src/network/responses/state_diff_response.h
        src/network/responses/full_state_response.cpp src/network/responses/full_state_response.h
        # network manager
        #  src/network/server_network_manager.cpp src/network/server_network_manager.h   # not needed in client
        # utils
        src/common/utils/json_utils.h
        src/common/utils/uuid_generator.h
        src/common/utils/global_state.h
        # reactive state
        src/reactive_state/serializable.h
        src/reactive_state/timestamp.h
        src/reactive_state/diffs/value_diff.h
        src/reactive_state/array_helpers.h
        src/reactive_state/value_type_helpers.h
        src/reactive_state/diffable_utils.h
        src/reactive_state/reactive_objects/reactive_value.h
        src/reactive_state/reactive_objects/reactive_object.cpp src/reactive_state/reactive_objects/reactive_object.h
        src/reactive_state/diffs/object_diff.cpp src/reactive_state/diffs/object_diff.h
        src/reactive_state/diffs/diff.cpp src/reactive_state/diffs/diff.h
        src/reactive_state/diffs/array_elem_diff.cpp src/reactive_state/diffs/array_diff.h
        src/client/app/Lama.cpp src/client/app/Lama.h src/client/GameController.cpp src/client/GameController.h src/client/panels/MainGamePanel.cpp src/client/panels/MainGamePanel.h)
add_executable(Lama-client ${SOURCE_FILES})

target_compile_definitions(Lama-client PRIVATE LAMA_CLIENT=1 RAPIDJSON_HAS_STDSTRING=1)

target_link_libraries(Lama-client ${wxWidgets_LIBRARIES})

# linking to sockpp
if(WIN32)
    message("Detected Win32")

    target_link_libraries(Lama-client ${CMAKE_SOURCE_DIR}/sockpp/cmake-build-debug/sockpp-static.lib)

    # Necessary to get sockets working under Windows (with MingW)
    target_link_libraries(Lama-client wsock32 ws2_32)
else()
    message("Not Win32, so probably a Linux") # We assume it's Linux in this case

    set(THREADS_PREFER_PTHREAD_FLAG ON)
    find_package(Threads REQUIRED)

    target_link_libraries(Lama-client ${CMAKE_SOURCE_DIR}/sockpp/cmake-build-debug/libsockpp.so Threads::Threads)
endif()

# copy assets (images) to binary directory
file(INSTALL assets DESTINATION ${CMAKE_BINARY_DIR})