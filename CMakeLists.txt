# Minimum CMake version required, we'll just use the latest version.
cmake_minimum_required(VERSION 3.22)
# Project name, version and description
project(discord-bot VERSION 1.0 DESCRIPTION "A discord bot")
 
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
 
# Create an executable
add_executable(${PROJECT_NAME}
    src/main.cpp
    src/withered.cpp
)
 
# Find our pre-installed DPP package (using FindDPP.cmake).
find_package(DPP REQUIRED)
find_package(nlohmann_json 3.12.0 REQUIRED)


# Link the pre-installed DPP package.
target_link_libraries(${PROJECT_NAME} 
    ${DPP_LIBRARIES}
    nlohmann_json::nlohmann_json)

# Include the DPP directories.
target_include_directories(${PROJECT_NAME} PRIVATE
    ${DPP_INCLUDE_DIR}
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
)
 
# Set C++ version
set_target_properties(${PROJECT_NAME} PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
)

# Get Token File
get_filename_component(TOKEN_FILE $ENV{HOME}/.config/discord_bot/TOKEN.txt ABSOLUTE)
get_filename_component(WITHERED_FILE $ENV{HOME}/.config/discord_bot/json/withered.json ABSOLUTE)
get_filename_component(CONFIG_FILE $ENV{HOME}/.config/discord_bot/config.json ABSOLUTE)

target_compile_definitions(discord-bot PRIVATE TOKEN_FILE_PATH="${TOKEN_FILE}" PRIVATE WITHERED_FILE_PATH="${WITHERED_FILE}" PRIVATE CONFIG_FILE_PATH="${CONFIG_FILE}")
