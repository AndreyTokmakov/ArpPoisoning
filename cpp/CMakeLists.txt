cmake_minimum_required(VERSION 3.16 FATAL_ERROR)

project(arp_spoof VERSION 1.0.0.0 LANGUAGES C CXX)
# set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}")
project(${PROJECT_NAME} VERSION ${PROJECT_VERSION} LANGUAGES CXX)

# set(CMAKE_CXX_STANDARD 23)

add_compile_options(-c -Wall -Wextra -O3 -std=c++2a)

# include all components
add_executable(arp_spoof 
    main.cpp
    Headers/ARPHeader.h
    Headers/EthernetHeader.h
    Headers/ICMPHeader.h
    Headers/IPv4Header.h
    Headers/TCPHeader.h
    Headers/UDPHeader.h
    Utilities/Utilities.h
    Utilities/Utilities.cpp
)

TARGET_LINK_LIBRARIES(arp_spoof)