cmake_minimum_required(VERSION 3.10)
project(NetLyzer VERSION 1.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find the required Qt packages
find_package(Qt5 REQUIRED COMPONENTS Widgets)

# Include Qt include directories
include_directories(${Qt5Widgets_INCLUDE_DIRS})

# Include directories
include_directories(src)

# Source files
set(SOURCES
    src/main.cpp
    src/gui/mainwindow.cpp
    src/gui/packetview.cpp
    src/network/packet_sniffer.cpp
    src/network/packet_parser.cpp
    src/utils/logger.cpp
)

# Header files (for MOC - Meta-Object Compiler)
set(HEADERS
    src/gui/mainwindow.h
    src/gui/packetview.h
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

# Link Qt libraries
target_link_libraries(NetLyzer Qt5::Widgets)