cmake_minimum_required(VERSION 3.7)

project(gfx 
    LANGUAGES CXX C
    VERSION 0.0.1
    )

set(CMAKE_CXX_STANDARD 17)

set(GFX_BUILD_EXAMPLES ON)
set(GFX_VALIDATION ON CACHE BOOL  "Enable validation for shaders and other useful things. Disable for optimization")

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

find_package(glfw3 3.3 REQUIRED)
find_package(glm 0.9 REQUIRED)
find_package(fmt 7.0 REQUIRED)
find_package(spdlog 1.8 REQUIRED)
find_package(Freetype 2.10 REQUIRED)

include(GenerateExportHeader)
include(GNUInstallDirs)

file(GLOB SOURCES src/*.cpp src/*.c src/*.hpp src/imgui/*.cpp src/imgui/*.h src/debug_draw/*.hpp)
file(GLOB HEADERS include/gfx/*.hpp include/gfx/*.h)

add_library(gfx STATIC ${HEADERS} ${SOURCES})
target_link_libraries(gfx ${CMAKE_DL_LIBS} glfw glm fmt::fmt spdlog::spdlog Freetype::Freetype)
target_include_directories(gfx
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

if(GFX_VALIDATION)
    target_compile_definitions(gfx PRIVATE GFX_VALIDATION)
endif()


add_subdirectory(examples)

install(
    TARGETS gfx
    EXPORT gfxTarget
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBRARYDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBRARYDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

export(
    EXPORT gfxTarget
    FILE ${CMAKE_CURRENT_BINARY_DIR}/gfx/gfxTargets.cmake
)

install(
    FILES ${HEADERS}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    COMPONENT devel
)

install(
    EXPORT gfxTarget
    DESTINATION lib/gfx
    FILE gfxTarget.cmake
    COMPONENT devel
)

install(
    FILES gfxConfig.cmake
    DESTINATION lib/gfx
    COMPONENT devel
)