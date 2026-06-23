cmake_minimum_required(VERSION 3.25)
project(DIY_Sim_Racing_Setup)

set(CMAKE_CXX_STANDARD 17)

add_executable(DIY_Sim_Racing_Setup pedal.cpp serialib.cpp serialib.h)


target_include_directories(DIY_Sim_Racing_Setup PRIVATE
        ${CMAKE_SOURCE_DIR}/third-party/vjoy/inc
        )

target_link_directories(DIY_Sim_Racing_Setup PRIVATE
        ${CMAKE_SOURCE_DIR}/third-party/vjoy/lib/amd64
        )

target_link_libraries(DIY_Sim_Racing_Setup PRIVATE
        "${CMAKE_SOURCE_DIR}/third-party/vjoy/lib/amd64/vJoyInterface.lib"
        )