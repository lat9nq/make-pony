add_executable(make-pony
    color.c
    color.h
    make-pony.c
    make-pony.glade
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

target_include_directories(make-pony PRIVATE ${GTK_INCLUDE_DIRS})

target_link_options(make-pony PUBLIC -export-dynamic)
    
target_link_libraries(make-pony PRIVATE m PNG::PNG ${GTK_LIBRARIES} Threads::Threads)
