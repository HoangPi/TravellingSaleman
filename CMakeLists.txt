find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

set(
    SOURCES
    main.cpp
    utility/mathLib.cpp utility/CanvasInteraction/CanvasInteraction.cpp
    utility/classes/Edge.cpp
    utility/classes/Vertex.cpp
)

add_executable(main main.cpp utility/mathLib.cpp utility/CanvasInteraction/CanvasInteraction.cpp)
target_link_libraries(main ${OpenCV_LIBS})
