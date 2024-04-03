cmake_minimum_required(VERSION 3.0.0)
project(PSO_MODEL VERSION 0.1.0 LANGUAGES C CXX)

include(CTest)
enable_testing()

include_directories(inc)
file(GLOB SOURCES "src/*.cpp")

add_executable(PSO_MODEL ${SOURCES})

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
