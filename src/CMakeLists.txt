set(BINARY ${CMAKE_PROJECT_NAME})

# add executables
file(GLOB_RECURSE SOURCES LIST_DIRECTORIES true *.h *.cc)

set(SOURCES ${SOURCES})

add_executable(${BINARY}_run ${SOURCES}
        main.cc)

add_library(${BINARY}_lib STATIC ${SOURCES})