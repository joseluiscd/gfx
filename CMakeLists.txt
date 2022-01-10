cmake_minimum_required(VERSION 3.9)

project(gfx 
    LANGUAGES CXX C
    VERSION 0.0.1
    DESCRIPTION "A low-level Graphics Library abstraction layer (OpenGL 4.5)"
    )

set(CMAKE_CXX_STANDARD 17)

option(GFX_BUILD_DOC "Build documentation" OFF)
option(GFX_BUILD_EXAMPLES "Build examples" ON)
option(GFX_VALIDATION "Enable validation for shaders and other useful things. Disable for optimization" ON)
option(GFX_IMGUI_FREETYPE "Use FreeType as TTF rasterizer instead of STB in ImGui fonts" ON)
option(GFX_ENABLE_FONTAWESOME "Build FontAwesome fonts into ImGui." ON)
option(GFX_ENABLE_LATO_FONT "Build Lato font into ImGui" ON)

include(GNUInstallDirs)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

find_package(glfw3 3.3 REQUIRED)
find_package(glm REQUIRED)
find_package(Freetype 2.10)
find_package(Doxygen)

# Some platforms define glm instead of glm::glm
if (TARGET glm AND NOT TARGET glm::glm)
    add_library(glm::glm ALIAS glm)
endif()

include(GenerateExportHeader)
include(GNUInstallDirs)

file(GLOB SOURCES src/*.cpp src/*.c src/*.hpp src/imgui/*.cpp src/imgui/*.h src/debug_draw/*.hpp)
file(GLOB HEADERS include/gfx/*.hpp include/gfx/*.h)

# Some configuration options
set(DEFINES)
set(LIBRARIES)
if (GFX_IMGUI_FREETYPE AND Freetype_FOUND)
    message("Using Freetype as ImGui font rasterizer")
    list(APPEND DEFINES IMGUI_ENABLE_FREETYPE)
    list(APPEND SOURCES src/imgui_freetype/imgui_freetype.cpp)
    list(APPEND LIBRARIES Freetype::Freetype)
endif()
if (GFX_ENABLE_FONTAWESOME)
    message("Building fontawesome font into imgui")
    list(APPEND DEFINES GFX_ENABLE_FONTAWESOME)
    list(APPEND SOURCES src/fonts/font_awesome.cpp src/fonts/font_awesome_2.cpp)
endif()
if (GFX_ENABLE_LATO_FONT)
    message("Building lato font into imgui")
    list(APPEND DEFINES GFX_ENABLE_LATO)
    list(APPEND SOURCES src/fonts/font_lato.cpp)
endif()

add_library(gfx STATIC ${HEADERS} ${SOURCES})
target_link_libraries(gfx PUBLIC ${CMAKE_DL_LIBS} glfw glm::glm ${LIBRARIES})
target_include_directories(gfx
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
target_compile_definitions(gfx
    PRIVATE ${DEFINES}
)

if(GFX_VALIDATION)
    target_compile_definitions(gfx PUBLIC GFX_VALIDATION)
endif()

include(CMakePackageConfigHelpers)
configure_package_config_file(${CMAKE_CURRENT_SOURCE_DIR}/gfxConfig.cmake.in
    "${CMAKE_CURRENT_BINARY_DIR}/gfxConfig.cmake"
    INSTALL_DESTINATION "lib/cmake/gfx"
    NO_SET_AND_CHECK_MACRO
    NO_CHECK_REQUIRED_COMPONENTS_MACRO
    )
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/gfxConfigVersion.cmake"
    VERSION ${CMAKE_PROJECT_VERSION} 
    COMPATIBILITY AnyNewerVersion
    )

install(
    TARGETS gfx
    
    EXPORT gfxTargets
    )
export(EXPORT gfxTargets)
install(EXPORT gfxTargets DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/cmake/gfx)
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/gfx DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/gfxConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/gfxConfigVersion.cmake
    DESTINATION lib/cmake/gfx
    ) 
    
add_subdirectory(examples)
add_subdirectory(doc)