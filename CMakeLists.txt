cmake_minimum_required(VERSION 3.9 FATAL_ERROR)
project(sonocarte)

enable_language(C CXX)
set(CMAKE_CXX_STANDARD 17)

find_package(SndFile REQUIRED)
find_package(ALSA REQUIRED)


set (APP_CXX_SRC
    ${PROJECT_SOURCE_DIR}/alsa_port.cpp
    ${PROJECT_SOURCE_DIR}/sndfile_port.cpp
    ${PROJECT_SOURCE_DIR}/sonocarte.cpp
)

add_executable(${CMAKE_PROJECT_NAME}.elf ${APP_CXX_SRC})
target_link_libraries(${CMAKE_PROJECT_NAME}.elf SndFile::sndfile ALSA::ALSA pthread m)


