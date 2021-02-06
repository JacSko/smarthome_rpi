




add_library(loggerMock STATIC
			mocks/logger_mock.hpp
)
target_include_directories(loggerMock PUBLIC
            mocks
            ../include
)
set_target_properties(loggerMock PROPERTIES LINKER_LANGUAGE CXX)