cmake_minimum_required(VERSION 3.8)
project(MemoryMQ)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
        Delegete.cpp
        Delegete.h
        dllmain.cpp
        MemoryMessage.cpp
        MemoryMessage.h
        MessageQueue.cpp
        MessageQueue.h
        stdafx.cpp
        stdafx.h
        targetver.h)

add_executable(MemoryMQ ${SOURCE_FILES})