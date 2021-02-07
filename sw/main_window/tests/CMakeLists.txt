add_library(MainWindowWrapperMock STATIC
			mocks/MainWindowWrapperMock.h
)
target_include_directories(MainWindowWrapperMock PUBLIC
            mocks
            ../include
)
set_target_properties(MainWindowWrapperMock PROPERTIES LINKER_LANGUAGE CXX)