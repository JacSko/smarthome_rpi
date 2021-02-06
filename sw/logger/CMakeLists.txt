cmake_minimum_required(VERSION 3.1.0)

if (NOT UNIT_TESTS)

add_library(Logger
	source/Logger.cpp
)
target_include_directories(Logger PUBLIC
	public/
	include/
)

else()

add_subdirectory(tests)

endif()