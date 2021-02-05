cmake_minimum_required(VERSION 3.1.0)

if (NOT UNIT_TESTS)

add_library(SocketDriver
	source/SocketDriver.cpp
)
target_include_directories(SocketDriver PUBLIC
	public/
	include/
)

else()

	add_subdirectory(tests)
endif()