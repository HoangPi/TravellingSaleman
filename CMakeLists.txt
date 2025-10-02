cmake_minimum_required(VERSION 3.5)
project(OpenCVQtApp)

# Enable Qt features
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Find OpenCV
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})
set(PROGRAM_NAME main)

# Source files
set(SOURCES
    main.cpp
)

# Create executable
add_executable(${PROGRAM_NAME} ${SOURCES})

# Link OpenCV libraries
target_link_libraries(${PROGRAM_NAME}
    ${OpenCV_LIBS}
)
