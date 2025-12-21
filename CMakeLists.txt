cmake_minimum_required(VERSION 3.28)
project(Bookstore)

set(CMAKE_CXX_STANDARD 14)

include_directories(include)
include_directories(src)

add_executable(code
        include/AccountSystem.h
        include/storage.h
        src/AccountSystem.cpp
        src/storage.cpp
        include/BookSystem.h
        src/BookSystem.cpp
        include/FinanceSystem.h
        src/FinanceSystem.cpp
        src/main.cpp)
