execute_process(
    COMMAND
        tr "\n" " "
    COMMAND
        sed "s/  */ /g"
    OUTPUT_VARIABLE
        MAKE_PONY_GLADE
    INPUT_FILE
        ${CMAKE_CURRENT_SOURCE_DIR}/make-pony.glade
)
configure_file(make-pony.glade.h.in make-pony.glade.h ESCAPE_QUOTES)

add_executable(make-pony
    color.c
    color.h
    make-pony.c
    make-pony.glade
    make-pony.glade.h.in
    make-pony.h
    nbt.c
    nbt.h
    pixel.c
    pixel.h
    pngimg.c
    pngimg.h
    target.c
    target.h
    thumbnailer.c
    thumbnailer.h)

target_compile_options(make-pony BEFORE PUBLIC
    -pipe
    -Wall
    -Wextra)

include_directories(.)
include_directories(${CMAKE_CURRENT_BINARY_DIR})
target_include_directories(make-pony PRIVATE ${GTK3_INCLUDE_DIRS})

if (NOT WIN32)
    target_link_options(make-pony PUBLIC -export-dynamic)
else()
    message(STATUS "Using Windows compile options")
    #~ target_link_options(make-pony PRIVATE -export-all-symbols)
    target_compile_options(make-pony PRIVATE -mwindows)
endif()

target_link_libraries(make-pony PRIVATE m PNG::PNG ${GTK3_LIBRARIES} Threads::Threads)
