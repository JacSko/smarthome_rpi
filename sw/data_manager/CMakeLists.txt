cmake_minimum_required(VERSION 3.1.0)

if (NOT UNIT_TESTS)

add_library(SocketDriver
	source/SocketDriver.cpp
)
target_include_directories(SocketDriver PUBLIC
	public/
	include/
)
target_link_libraries(SocketDriver PUBLIC
	Logger
)


add_library(DataProvider
	source/DataProvider.cpp
)
target_include_directories(DataProvider PUBLIC
	public/
	include/
)
target_link_libraries(DataProvider PUBLIC
	SocketDriver
	Logger
	MainWindowIf
	SmartHomeTypes
)

else()

	add_subdirectory(tests)
endif()