
cmake_minimum_required(VERSION 3.21)

project(EmergencyAmbulanceSystem
    VERSION 1.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Sql)

qt_standard_project_setup()

qt_add_executable(EmergencyAmbulanceSystem
    main.cpp
)

target_link_libraries(EmergencyAmbulanceSystem
    PRIVATE
    Qt6::Widgets
    Qt6::Sql
)

set_target_properties(EmergencyAmbulanceSystem PROPERTIES
    WIN32_EXECUTABLE ON
)
