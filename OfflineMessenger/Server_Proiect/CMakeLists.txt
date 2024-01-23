cmake_minimum_required(VERSION 3.26)
project(Server_Proiect)

set(CMAKE_CXX_STANDARD 17)

add_executable(Server_Proiect main.cpp
        Messenger.h
        Library.h
        Files.h
        MessImplement.h)
target_link_libraries(Server_Proiect pthread)
