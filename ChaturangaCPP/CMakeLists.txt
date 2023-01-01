cmake_minimum_required(VERSION 3.16)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED true)
set(COMPILE_WARNING_AS_ERROR true)
# add_compile_options(-Wall -Wextra -pedantic -Werror -Wno-vla)

project(chaturanga)

add_executable(chaturanga
	Ashtapada.cpp  Chal.cpp  Chaturanga.cpp  Evaluator.cpp  NitiAyog.cpp  Position.cpp  pieces.cpp
	Ashtapada.h  Evaluator.h  LinkedList.h  Position.h      Store.h  pieces.h
	Chal.h       LRU.h        NitiAyog.h    ReleaseCheck.h  storage.h
)


