cmake_minimum_required(VERSION 3.10)

# set the project name and version
project(make-pony VERSION 1.4.2)

configure_file(make-pony.h.in make-pony.h)

target_include_directories(make-pony PUBLIC
                           "${PROJECT_BINARY_DIR}"
                           )
