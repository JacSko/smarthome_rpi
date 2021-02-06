########################################
#	Build instructions:
#
# - Prepare cross compiler (have to be placed in /opt/)
#		wget https://github.com/Pro/raspi-toolchain/releases/latest/download/raspi-toolchain.tar.gz
#		sudo tar xfz raspi-toolchain.tar.gz --strip-components=1 -C /opt
#
# - Cross-compile QT for Raspberry (5.12.5)
#		https://mechatronicsblog.com/cross-compile-and-deploy-qt-5-12-for-raspberry-pi/
#
# - To build on Ubuntu, You have to have QT5 compiled additionaly (QT for Raspberry wouldn't work)
#
#	Adjust paths below (Raspsberry QT libs and CC toolchain)
#
#
cmake_minimum_required(VERSION 3.1.0)

project(smarthome_rpi VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
if(CMAKE_VERSION VERSION_LESS "3.7.0")
    set(CMAKE_INCLUDE_CURRENT_DIR ON)
endif()

#########################################
#	Below paths should be adjusted
#########################################
set (QT_RPI_LIBRARIES_PATH "/home/jskowronek/raspi/qt5pi")
set (RPI_TOOLCHAIN_PATH "/opt/cross-pi-gcc/bin/")


if (NOT UNIT_TESTS)
	#########################################
	#	RaspberryPi compiler settings
	#########################################
	if (BUILD_RPI)
		set (CMAKE_PREFIX_PATH ${QT_RPI_LIBRARIES_PATH})
		set(CMAKE_C_COMPILER   ${RPI_TOOLCHAIN_PATH}/arm-linux-gnueabihf-gcc)
		set(CMAKE_CXX_COMPILER ${RPI_TOOLCHAIN_PATH}/arm-linux-gnueabihf-g++)
		set(CMAKE_LINKER       ${RPI_TOOLCHAIN_PATH}/arm-linux-gnueabihf-ld CACHE PATH "" FORCE)
	endif()
	
	
	#########################################
	#	QT package is required to run app
	#########################################
	set(CMAKE_AUTOMOC ON)
	set(CMAKE_AUTORCC ON)
	set(CMAKE_AUTOUIC ON)
	find_package(Qt5 COMPONENTS Widgets REQUIRED)
	
	#########################################
	#	Main executable
	#########################################
	add_executable(smarthome_rpi
	    sw/gui/main_window.ui
	    sw/main_window/source/main_window.cpp
	    sw/main_window/include/main_window.h
	    sw/main.cpp
	    sw/gui/resources.qrc
	)
	target_include_directories(smarthome_rpi PUBLIC
		sw/main_window/include
	)
	target_link_libraries(smarthome_rpi 
		Qt5::Widgets
		pthread
		)
	#########################################
	#	TEST executable
	#########################################
	add_executable(smarthome_test
	    sw/main_test.cpp
	)
	target_include_directories(smarthome_test PUBLIC
		
	)
	target_link_libraries(smarthome_test 
		SocketDriver
		Logger
		pthread
		)		
		
else()
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
	set(CMAKE_CXX_FLAGS "-g -O0 -Wall -fprofile-arcs -ftest-coverage -fPIC")
	add_definitions(
		-DUNIT_TESTS
		)
	add_subdirectory(ext_libs/googletest)
	enable_testing()
if (LOGGER_DISABLE_OUTPUT)
	add_definitions(-DLOGGER_DISABLE_OUTPUT)
endif()

endif()

add_subdirectory(sw/data_manager)
add_subdirectory(sw/logger)








