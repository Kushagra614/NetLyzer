cmake_minimum_required(VERSION 3.10)
project(NetLyzer VERSION 1.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find the required Qt packages
find_package(Qt5 REQUIRED COMPONENTS Widgets)

# Find libpcap
find_package(PkgConfig REQUIRED)
pkg_check_modules(PCAP REQUIRED libpcap)

# Include directories
include_directories(${Qt5Widgets_INCLUDE_DIRS})
include_directories(${PCAP_INCLUDE_DIRS})
include_directories(src)

# Source files
set(SOURCES
    src/main.cpp
    src/gui/main_window.cpp
    src/network/packet_sniffer.cpp
    src/network/packet_parser.cpp
    src/utils/logger.cpp
)

# Header files (for MOC - Meta-Object Compiler)
set(HEADERS
    src/gui/main_window.h
    src/network/packet_sniffer.h
    src/network/packet_parser.h
    src/utils/logger.h
)

# UI files
set(UIS
    src/gui/mainwindow.ui
)

# Generate MOC and UI files
qt5_wrap_cpp(MOC_SOURCES ${HEADERS})
qt5_wrap_ui(UI_HEADERS ${UIS})

# Add executable
add_executable(NetLyzer ${SOURCES} ${MOC_SOURCES} ${UI_HEADERS})

# Link libraries
target_link_libraries(NetLyzer Qt5::Widgets ${PCAP_LIBRARIES})
