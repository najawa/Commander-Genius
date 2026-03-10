
include_directories(${SDL_INCLUDE_DIR})


set(CXXSRC
    dreamscontrolpanel.cpp
    dreamsdosintro.cpp
    dreamsengine.cpp
    dreamsgameplay.cpp
    dreamsintro.cpp
    )

set(HSRC
    dreamscontrolpanel.h
    dreamsdosintro.h
    dreamsengine.h
    dreamsgameplay.h
    dreamsintro.h
    )

# Suppress warnings from vendored refkeen headers included by dreams code
add_definitions(-Wno-deprecated-declarations)

add_library(engine_keen_dreams STATIC ${CXXSRC} ${HSRC})

target_link_libraries(engine_keen_dreams refkeen engine_keen)
