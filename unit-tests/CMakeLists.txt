project(Tichu-unit-tests)

set(TEST_SOURCE_FILES
        serialization.cpp
        combi.cpp
)

add_executable(Tichu-tests ${TEST_SOURCE_FILES})

target_link_libraries(Tichu-tests gtest gtest_main Tichu-lib)