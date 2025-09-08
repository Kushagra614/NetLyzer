cmake_minimum_required(VERSION 3.10)
project(NetLyzer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required packages
find_package(PkgConfig REQUIRED)
pkg_check_modules(PCAP REQUIRED libpcap)

# Include directories
include_directories(
    ${CMAKE_SOURCE_DIR}/include
    ${PCAP_INCLUDE_DIRS}
)

# Add executable
add_executable(netlyzer 
    src/main.cpp
)

# Link libraries
target_link_libraries(netlyzer 
    ${PCAP_LIBRARIES} 
    pthread
)
