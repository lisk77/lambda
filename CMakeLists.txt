cmake_minimum_required(VERSION 3.10)
project(lambda VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 1) gather all your .cpp files
file(GLOB_RECURSE PROJECT_SOURCES
     ${PROJECT_SOURCE_DIR}/src/*.cpp
)

# sanity check
if(NOT PROJECT_SOURCES)
  message(FATAL_ERROR "No .cpp files found in src/")
endif()

# 2) create the executable from those sources
add_executable(${PROJECT_NAME} ${PROJECT_SOURCES})

# 3) point it at your headers
target_include_directories(${PROJECT_NAME}
  PRIVATE ${PROJECT_SOURCE_DIR}/include
)

