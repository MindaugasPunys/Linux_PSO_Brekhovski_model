cmake_minimum_required(VERSION 3.0.0)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pg")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pg")

project(PSO_MODEL VERSION 0.1.0 LANGUAGES C CXX)

include(CTest)
enable_testing()

include_directories(inc)
file(GLOB SOURCES "src/*.cpp")

add_executable(PSO_MODEL ${SOURCES})

# Include the FFTW library
include_directories("/usr/include")
target_link_libraries(PSO_MODEL "/usr/lib/x86_64-linux-gnu/libfftw3f.so")

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
