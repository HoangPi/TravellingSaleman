find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(main main.cpp utility/mathLib.cpp utility/CanvasInteraction/CanvasInteration.cpp)
target_link_libraries(main ${OpenCV_LIBS})