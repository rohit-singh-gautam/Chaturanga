cmake_minimum_required(VERSION 3.16)

# add_compile_options(-Wall -Wextra -pedantic -Werror -Wno-vla)
add_compile_options(-Wall -Wextra -pedantic -Werror -Wno-vla)

project(chaturanga VERSION 1.0)

include_directories(chaturanga
    $(CMAKE_CURRENT_SOURCE_DIR)/include
    ${CMAKE_CURRENT_BINARY_DIR}/include
    )

add_executable(chaturanga
	Ashtapada.cpp  Chal.cpp  Chaturanga.cpp  Evaluator.cpp  NitiAyog.cpp  Position.cpp  pieces.cpp commandline.cpp
	include/Ashtapada.h include/Evaluator.h include/LinkedList.h include/Position.h
	include/Store.h include/pieces.h include/Chal.h include/LRU.h include/NitiAyog.h
	include/ReleaseCheck.h include/storage.h
)

configure_file(include/version.h.in include/version.h)

target_compile_features(chaturanga PRIVATE cxx_std_23)
