project(Tichu-unit-tests)

set(TEST_SOURCE_FILES
        card.cpp
        hand.cpp)

add_executable(Tichu-tests ${TEST_SOURCE_FILES})

target_compile_definitions(Tichu-tests PRIVATE TICHU_SERVER=1 RAPIDJSON_HAS_STDSTRING=1)

target_link_libraries(Tichu-tests gtest gtest_main Tichu-lib)