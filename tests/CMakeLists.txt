# Add test executable
add_executable(netlyzer_tests
    test_packet_parser.cpp
    test_packet_sniffer.cpp
)

# Link with the main library and Google Test
target_link_libraries(netlyzer_tests
    PRIVATE
        netlyzer_lib
        GTest::GTest
        GTest::Main
)

# Add test
target_include_directories(netlyzer_tests
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/..
)

# Discover and add tests
include(GoogleTest)
gtest_discover_tests(netlyzer_tests)
