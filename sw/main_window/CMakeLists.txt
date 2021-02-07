
if (NOT UNIT_TESTS)


add_library(MainWindowIf INTERFACE)

target_include_directories(MainWindowIf INTERFACE
			public/
)

else()

	add_subdirectory(tests)

endif()