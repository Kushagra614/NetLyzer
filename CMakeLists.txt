cmake_minimum_required(VERSION 3.16)
project(NetLyzer VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required packages
find_package(Qt6 REQUIRED COMPONENTS Core Widgets Network)
find_package(PkgConfig REQUIRED)
pkg_check_modules(PC_LIBPCAP REQUIRED IMPORTED_TARGET libpcap)

# Enable Qt's MOC, UIC, and RCC
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Create a simple working version first
add_executable(netlyzer 
    src/main.cpp
    src/network/packet_parser.cpp
)

# Include directories
target_include_directories(netlyzer PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${PC_LIBPCAP_INCLUDE_DIRS}
)

# Link libraries
target_link_libraries(netlyzer PRIVATE 
    Qt6::Core
    Qt6::Widgets
    Qt6::Network
    PkgConfig::PC_LIBPCAP
    pthread
)

# Set compiler warnings
if(MSVC)
    target_compile_options(netlyzer PRIVATE /W4)
else()
    target_compile_options(netlyzer PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Install target
install(TARGETS netlyzer DESTINATION bin)
