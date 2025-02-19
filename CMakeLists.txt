cmake_minimum_required(VERSION 3.16)

project(chaturanga VERSION 1.0)

include_directories(chaturanga
    $(CMAKE_CURRENT_SOURCE_DIR)/include
    ${CMAKE_CURRENT_BINARY_DIR}/include
    )

add_executable(chaturanga
	Ashtapada.cpp  Chal.cpp  Chaturanga.cpp  Evaluator.cpp  NitiAyog.cpp  pieces.cpp commandline.cpp
	include/Ashtapada.h include/Evaluator.h include/LinkedList.h include/Position.h
	include/Store.h include/pieces.h include/Chal.h include/LRU.h include/NitiAyog.h
	include/ReleaseCheck.h include/storage.h
)

if (UNIX)
add_compile_options(-Wall -Werror -Wpedantic -Wextra -Weffc++ -Wsuggest-override) # -fanalyzer
elseif (MSVC)
add_compile_options(/W4 /WX)
endif()

configure_file(include/version.h.in include/version.h)

target_compile_features(chaturanga PRIVATE cxx_std_20)
