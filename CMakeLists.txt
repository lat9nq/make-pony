cmake_minimum_required(VERSION 3.10)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules")

# set the project name and version
project(make-pony VERSION 1.4.2)

#configure_file(make-pony.h.in make-pony.h)

find_package(PNG REQUIRED VERSION 1.16.0)

set(GTK_FIND_VERSION 3.0)
find_package(GTK REQUIRED)
if(NOT GTK_FOUND)
    message(FATAL_ERROR "GTK not found")
endif()
#message(STATUS "${GTK_INCLUDE_DIRS}")
#message(STATUS "${GTK_LIBRARIES}")

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
if (NOT Threads_FOUND)
    message(FATAL_ERROR "Threads not found")
endif()

add_subdirectory(src)
