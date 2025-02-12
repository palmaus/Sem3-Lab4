cmake_minimum_required(VERSION 3.28)
project(Sem3-Lab4)

set(CMAKE_CXX_STANDARD 20)

add_executable(Sem3-Lab4
        main.cpp
        DataStructures/Student.cpp
        Tests/exec/InternalTests.cpp
        Tests/exec/TestRunner.cpp
        GUI/GUI.cpp
)

target_include_directories(Sem3-Lab4 PUBLIC
        Sequences
        PTRs
        Sorts
        dict
        Vertex
        Edges
        Graph
        GraphGenerator
        GUI
        GraphAlgorithms
        Iterators
        DataStructures
        Tests/include
)

find_package(Qt6 REQUIRED COMPONENTS Widgets Gui Core)
target_link_libraries(Sem3-Lab4 PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets)
