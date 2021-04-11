project(Lama-tests)

set(TEST_SOURCE_FILES
        card.cpp
        hand.cpp)

add_executable(tests ${TEST_SOURCE_FILES})

target_compile_definitions(tests PRIVATE LAMA_SERVER=1 RAPIDJSON_HAS_STDSTRING=1)

target_link_libraries(tests gtest gtest_main Lama-lib)