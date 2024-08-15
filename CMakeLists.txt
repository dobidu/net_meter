cmake_minimum_required(VERSION 3.14)  # Minimum required version for FetchContent
project(net_meter)

set(CMAKE_CXX_STANDARD 17)

# Include directories
include_directories(include)

# Fetch CPR library
include(FetchContent)
FetchContent_Declare(
    cpr
    GIT_REPOSITORY https://github.com/libcpr/cpr.git
    GIT_TAG        master  # Use the desired branch or tag
)
FetchContent_MakeAvailable(cpr)

# Add the source files
set(SOURCES
    src/net_meter.cpp
    src/net_meter_http.cpp
    src/net_meter_ping.cpp
    src/main.cpp
)

# Create the executable
add_executable(net_meter ${SOURCES})

# Link the CPR library to the executable
target_link_libraries(net_meter PRIVATE cpr::cpr)
