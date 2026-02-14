cmake_minimum_required(VERSION 3.16.0 FATAL_ERROR)

project(Breakout C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

################################################################################
# Global configuration types
################################################################################
set(CMAKE_CONFIGURATION_TYPES
    "Debug"
    "Release"
    CACHE STRING "" FORCE
)

################################################################################
# Output directory
################################################################################
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/Bin")

################################################################################
# Sub-projects
################################################################################
add_subdirectory(Breakout)
