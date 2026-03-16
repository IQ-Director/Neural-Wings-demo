cmake_minimum_required(VERSION 3.11)

project(Neural_Wings-demo)


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
    add_compile_options("/utf-8")
    # /FS allows concurrent cl.exe processes to share the same PDB
    # (VS generators enable parallel builds by default).
    set(CMAKE_CXX_FLAGS_DEBUG "/MDd /Od /Z7 /EHsc /FS")
    set(CMAKE_EXE_LINKER_FLAGS_DEBUG "/DEBUG")
    set(CMAKE_CXX_FLAGS_RELEASE "/MD /O2 /DNDEBUG /EHsc /FS")
endif()


include(FetchContent)

FetchContent_Declare(raylib SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/third_party/raylib-master")
FetchContent_Declare(raygui SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/third_party/raygui-master")
FetchContent_Declare(nlohmann_json SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/third_party/json-develop")
FetchContent_Declare(pybind11 SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/third_party/pybind11-3.0.0")


set(ULTRALIGHT_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/third_party/ultralight")
set(NBNET_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/third_party/nbnet")

if(NOT EXISTS "${NBNET_ROOT}/nbnet.h")
    message(FATAL_ERROR
        "nbnet not found!  Please clone/download nbnet into:\n"
        "  ${NBNET_ROOT}\n"
        "  (needs nbnet.h and net_drivers/ folder)\n"
        "  https://github.com/nathhB/nbnet")
endif()

set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_GAMES OFF CACHE BOOL "" FORCE)


add_compile_definitions(SUPPORT_FILEFORMAT_HDR)
if(EMSCRIPTEN)
    set(OPENGL_VERSION "ES 3.0" CACHE STRING "OpenGL version" FORCE)
else()
    set(OPENGL_VERSION "3.3" CACHE STRING "OpenGL version" FORCE)
endif()

FetchContent_MakeAvailable(raylib raygui nlohmann_json pybind11)



file(GLOB_RECURSE SOURCES "src/*.cpp" "src/*.c")

file(REAL_PATH "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp" MAIN_SOURCE_PATH)
file(REAL_PATH "${CMAKE_CURRENT_SOURCE_DIR}/src/Game/AI/PythonBridge.cpp" PYTHON_BRIDGE_PATH)

set(CORE_SOURCES ${SOURCES})
list(REMOVE_ITEM CORE_SOURCES ${MAIN_SOURCE_PATH} ${PYTHON_BRIDGE_PATH})



if(EMSCRIPTEN)
    list(FILTER SOURCES EXCLUDE REGEX "src/Engine/UI/UltralightLayer.cpp")
else()
    list(FILTER SOURCES EXCLUDE REGEX "src/Engine/UI/WebLayer.cpp")
endif()


add_library(NW_Core STATIC ${CORE_SOURCES})
set_target_properties(NW_Core PROPERTIES LINKER_LANGUAGE CXX
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

target_include_directories(NW_Core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${raygui_SOURCE_DIR}/src
    ${NBNET_ROOT}
)


target_link_libraries(NW_Core PUBLIC
    raylib
    nlohmann_json::nlohmann_json
)


add_executable(${PROJECT_NAME} ${MAIN_SOURCE_PATH})
target_link_libraries(${PROJECT_NAME} PRIVATE NW_Core)

if(MSVC)
    # Ensure PDB is shared safely when cl.exe runs in parallel.
    target_compile_options(${PROJECT_NAME} PRIVATE /FS)
endif()

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${raygui_SOURCE_DIR}/src
    ${NBNET_ROOT}
)


if(EMSCRIPTEN)
    set(PLATFORM "Web")
    # add_definitions(-DPLATFORM_WEB)
    target_compile_definitions(${PROJECT_NAME} PRIVATE PLATFORM_WEB)
    message(STATUS "Web network backend: nbnet + WebRTC")
else()
    set(PLATFORM "Desktop")
    # add_definitions(-DPLATFORM_DESKTOP)
    target_compile_definitions(${PROJECT_NAME} PRIVATE PLATFORM_DESKTOP)
endif()

message(STATUS "当前平台: ${PLATFORM}")


if(PLATFORM STREQUAL "Web")
    set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".html")

    target_link_libraries(${PROJECT_NAME} PRIVATE
        raylib
        nlohmann_json::nlohmann_json
    )

    target_link_options(${PROJECT_NAME} PRIVATE
        "-sUSE_GLFW=3"
        "-sASYNCIFY"
        "-sUSE_WEBGL2=1"
        "-sMIN_WEBGL_VERSION=2"
        "-sMAX_WEBGL_VERSION=2"
        "-sFULL_ES3=1"

        "-sNO_DISABLE_EXCEPTION_CATCHING"

        "-sFORCE_FILESYSTEM=1"

        "-sALLOW_MEMORY_GROWTH=1"
        "-sINITIAL_MEMORY=268435456"

        "--js-library" "${CMAKE_CURRENT_SOURCE_DIR}/src/Engine/Network/Transport/nbnet_bridge.js"

        "--shell-file" "${CMAKE_CURRENT_SOURCE_DIR}/src/shell.html"

        "-sASSERTIONS=1"
        "-g3"
    )

    # Try to find npm to build the Vue UI automatically for web builds.
    find_program(NPM_EXECUTABLE npm)
    if(NPM_EXECUTABLE)
        add_custom_target(WebUI
            COMMAND ${NPM_EXECUTABLE} ci
            COMMAND ${NPM_EXECUTABLE} run build
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/ui
            COMMENT "Building Vue UI into ui/dist"
        )
        add_dependencies(${PROJECT_NAME} WebUI)
    else()
        message(STATUS "npm not found: skipping automatic UI build for Web. Run 'npm ci' and 'npm run build' in ui/ manually.")
    endif()

    # After building the Emscripten HTML, copy the generated ui/dist into the web build output
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/ui/dist"
        "${CMAKE_BINARY_DIR}/ui/dist"
    )

else()
    target_include_directories(NW_Core PUBLIC
        "${ULTRALIGHT_ROOT}/include"
    )

    target_include_directories(${PROJECT_NAME} PRIVATE
        ${ULTRALIGHT_ROOT}/include)

    target_link_libraries(NW_Core PUBLIC
        "${ULTRALIGHT_ROOT}/lib/Ultralight.lib"
        "${ULTRALIGHT_ROOT}/lib/UltralightCore.lib"
        "${ULTRALIGHT_ROOT}/lib/WebCore.lib"
        "${ULTRALIGHT_ROOT}/lib/AppCore.lib"
        raylib
        nlohmann_json::nlohmann_json
    )
    # target_link_libraries(${PROJECT_NAME} PRIVATE
    #     raylib
    #     nlohmann_json::nlohmann_json
    #     "${ULTRALIGHT_ROOT}/lib/Ultralight.lib"
    #     "${ULTRALIGHT_ROOT}/lib/UltralightCore.lib"
    #     "${ULTRALIGHT_ROOT}/lib/WebCore.lib"
    #     "${ULTRALIGHT_ROOT}/lib/AppCore.lib"
    # ) 
    target_link_libraries(${PROJECT_NAME} PRIVATE
        NW_Core
        raylib
        nlohmann_json::nlohmann_json
    )

    # nbnet UDP driver needs Winsock on Windows
    if(WIN32)
        target_link_libraries(NW_Core PUBLIC ws2_32 winmm)

        # Build Windows Desktop Release as a GUI app to avoid opening a console window.
        if(MSVC)
            set_target_properties(${PROJECT_NAME} PROPERTIES
                LINK_FLAGS_RELEASE "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")
        else()
            set_target_properties(${PROJECT_NAME} PROPERTIES
                LINK_FLAGS_RELEASE "-mwindows")
        endif()
    endif()

    target_link_libraries(${PROJECT_NAME} PRIVATE NW_Core)
    pybind11_add_module(nw_engine
        src/Game/AI/AIEnvironment.cpp
        src/Game/AI/PythonBridge.cpp
    )

    target_include_directories(nw_engine PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
    target_link_libraries(nw_engine PRIVATE NW_Core)

    if(WIN32)
        target_link_libraries(nw_engine PRIVATE ws2_32 winmm)
        target_link_options(nw_engine PRIVATE /FORCE:MULTIPLE)
    endif()


    set_target_properties(nw_engine PROPERTIES
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/ai_train"
    )
    # target_link_directories(nw_engine PRIVATE "${CMAKE_BINARY_DIR}/lib/Debug")

    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/assets"
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>/assets"
    )

    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${ULTRALIGHT_ROOT}/bin"
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
    )

    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${ULTRALIGHT_ROOT}/resources"
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>/resources"
    )

    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/ui"
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>/ui"
    )
endif()
