add_executable(MainWindowTests
            unit/MainWindowTests.cpp
            ../source/main_window.cpp
       	    ../../gui/main_window.ui
#		    sw/main_window/source/main_window.cpp
		    ../include/main_window.h
#		    sw/main.cpp
		    ../../gui/resources.qrc
)

target_include_directories(MainWindowTests PUBLIC
        ../include
        ../public
)
target_link_libraries(MainWindowTests PUBLIC
        gtest_main
        gmock_main
        loggerMock
        Qt5::Core
        Qt5::Widgets
        SmartHomeTypes
)
add_test(NAME MainWindowTests COMMAND MainWindowTests)





add_library(MainWindowWrapperMock STATIC
			mocks/MainWindowWrapperMock.h
)
target_include_directories(MainWindowWrapperMock PUBLIC
            mocks
            ../include
            ../public
)
set_target_properties(MainWindowWrapperMock PROPERTIES LINKER_LANGUAGE CXX)