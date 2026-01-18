cmake_minimum_required(VERSION 3.11)

project(Neural_Wings-demo)


set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if (MSVC)
    set(CMAKE_CXX_FLAGS_DEBUG "/MDd /Od /Zi /EHsc")
    set(CMAKE_CXX_FLAGS_RELEASE "/MD /O2 /DNDEBUG /EHsc")
endif()


include(FetchContent)
FetchContent_Declare(
    raylib
    GIT_REPOSITORY https://github.com/raysan5/raylib.git
    GIT_TAG master
)

FetchContent_Declare(
    raygui
    GIT_REPOSITORY https://github.com/raysan5/raygui.git
    GIT_TAG master
)

FetchContent_Declare(
    nlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.2
)

FetchContent_Declare(
  enet
  GIT_REPOSITORY https://github.com/lsalzman/enet.git
  GIT_TAG master
)

set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE) 
set(BUILD_GAMES OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(raylib raygui nlohmann_json enet)

file(GLOB_RECURSE SOURCES "src/*.cpp")
add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${raygui_SOURCE_DIR}/src
)

target_link_libraries(${PROJECT_NAME} PRIVATE raylib nlohmann_json::nlohmann_json )
include_directories(${CMAKE_SOURCE_DIR}/src)

if (EMSCRIPTEN)
    set(PLATFORM "Web")
    add_definitions(-DPLATFORM_WEB)
    
else()
    set(PLATFORM "Desktop")
    add_definitions(-DPLATFORM_DESKTOP)
endif()

message(STATUS "当前平台: ${PLATFORM}")
if (PLATFORM STREQUAL "Web")
    set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".html")

    target_link_options(${PROJECT_NAME} PRIVATE 
        "-sUSE_GLFW=3" 
        "-sASYNCIFY"
        "--shell-file" "${CMAKE_CURRENT_SOURCE_DIR}/src/shell.html"
        "--preload-file" "${CMAKE_CURRENT_SOURCE_DIR}/assets@assets"
    )

else()
    
    target_link_libraries(${PROJECT_NAME} PRIVATE enet)

    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_CURRENT_SOURCE_DIR}/assets"
    "$<TARGET_FILE_DIR:${PROJECT_NAME}>/assets"
)
endif()