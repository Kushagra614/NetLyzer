
cmake_minimum_required(VERSION 3.16)
project(NetLyzer VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required packages
find_package(Qt6 REQUIRED COMPONENTS Core Widgets)
find_package(PkgConfig REQUIRED)

# Find libpcap
pkg_check_modules(PCAP REQUIRED libpcap)

# Qt MOC
qt6_standard_project_setup()

# Source files
set(SOURCES
    src/main.cpp
    src/gui/main_window.cpp
    src/network/packet_sniffer.cpp
    src/network/packet_parser.cpp
    src/utils/logger.cpp
)

set(HEADERS
    src/gui/main_window.h
    src/network/packet_sniffer.h
    src/network/packet_parser.h
    src/utils/logger.h
)

# Create executable
qt6_add_executable(NetLyzer ${SOURCES} ${HEADERS})

# Link libraries
target_link_libraries(NetLyzer 
    Qt6::Core 
    Qt6::Widgets
    ${PCAP_LIBRARIES}
    pthread
)

# Include directories
target_include_directories(NetLyzer PRIVATE 
    ${PCAP_INCLUDE_DIRS}
    src/
)

# Compiler flags
target_compile_options(NetLyzer PRIVATE ${PCAP_CFLAGS_OTHER})

# Install target
install(TARGETS NetLyzer DESTINATION bin)
