cmake_minimum_required(VERSION 3.7)

project(gfx 
    LANGUAGES CXX C
    VERSION 0.0.1
    )

set(CMAKE_CXX_STANDARD 17)

option(GFX_BUILD_EXAMPLES "Build examples" ON)
option(GFX_VALIDATION "Enable validation for shaders and other useful things. Disable for optimization" ON)

include(GNUInstallDirs)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

find_package(glfw3 3.3 REQUIRED)
find_package(glm 0.9 REQUIRED)
find_package(fmt 7.0 REQUIRED)
find_package(Freetype 2.10 REQUIRED)

# Some platforms define glm instead of glm::glm
if (TARGET glm AND NOT TARGET glm::glm)
    add_library(glm::glm ALIAS glm)
endif()

include(GenerateExportHeader)
include(GNUInstallDirs)

file(GLOB SOURCES src/*.cpp src/*.c src/*.hpp src/imgui/*.cpp src/imgui/*.h src/debug_draw/*.hpp)
file(GLOB HEADERS include/gfx/*.hpp include/gfx/*.h)

add_library(gfx STATIC ${HEADERS} ${SOURCES})
target_link_libraries(gfx PUBLIC ${CMAKE_DL_LIBS} glfw glm::glm fmt::fmt Freetype::Freetype)
target_include_directories(gfx
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
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
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/gfx
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