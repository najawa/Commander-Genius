project(GsKit_imgui)

find_package(SDL2 REQUIRED)

set(IMGUI_SOURCES
    imgui.cpp
    imgui_draw.cpp
    imgui_tables.cpp
    imgui_widgets.cpp
    imgui_demo.cpp
    imgui_impl_sdl2.cpp
    imgui_impl_sdlrenderer2.cpp
)

add_library(${PROJECT_NAME} STATIC ${IMGUI_SOURCES})

target_include_directories(${PROJECT_NAME}
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
    PUBLIC ${SDL2_INCLUDE_DIR}
)

# Suppress warnings in vendored code
target_compile_options(${PROJECT_NAME} PRIVATE -w)
