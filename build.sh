#!/bin/bash

# Create build directory if it doesn't exist
mkdir -p build

# Navigate to build directory
cd build

# Run CMake and make
cmake ..
make

# Check if make was successful
if [ $? -eq 0 ]; then
    echo -e "\nBuild successful! You can run the program with:\n"
    echo "sudo ./netlyzer"
    echo -e "\nNote: Root privileges are required for packet capture."
else
    echo -e "\nBuild failed. Please check the error messages above."
    exit 1
fi
