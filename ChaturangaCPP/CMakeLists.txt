cmake_minimum_required(VERSION 3.16)

project(chaturanga)

add_executable(chaturanga
	Ashtapada.cpp  Chal.cpp  Chaturanga.cpp  Evaluator.cpp  NitiAyog.cpp  Position.cpp  pieces.cpp
	Ashtapada.h  Evaluator.h  LinkedList.h  Position.h      Store.h  pieces.h
	Chal.h       LRU.h        NitiAyog.h    ReleaseCheck.h  storage.h
)

set_property(TARGET chaturanga PROPERTY CXX_STANDARD 20)

