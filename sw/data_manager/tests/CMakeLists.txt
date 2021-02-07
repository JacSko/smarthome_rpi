add_executable(SocketDriverTests
            unit/SocketDriverTests.cpp
            ../source/SocketDriver.cpp
)

target_include_directories(SocketDriverTests PUBLIC
        ../include
        ../public
)
target_link_libraries(SocketDriverTests PUBLIC
        gtest_main
        gmock_main
        loggerMock
)

add_test(NAME SocketDriverTests COMMAND SocketDriverTests)












add_library(SocketDriverMock STATIC
			mocks/SocketDriverMock.h
)
target_include_directories(SocketDriverMock PUBLIC
            mocks
            ../include
)
set_target_properties(SocketDriverMock PROPERTIES LINKER_LANGUAGE CXX)