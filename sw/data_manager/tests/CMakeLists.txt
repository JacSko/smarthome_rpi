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
)

add_test(NAME SocketDriverTests COMMAND SocketDriverTests)
