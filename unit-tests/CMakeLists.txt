project(Lama-tests)

set(TEST_SOURCE_FILES
        card.cpp)

add_executable(tests ${TEST_SOURCE_FILES})

target_link_libraries(tests gtest gtest_main Lama-lib)