
CMAKE_MINIMUM_REQUIRED (VERSION 2.8)

PROJECT(taro)

# include directories
INCLUDE_DIRECTORIES(
	${PROJECT_SOURCE_DIR}/test/googletest/include
	${PROJECT_SOURCE_DIR}/include/common
	${PROJECT_SOURCE_DIR}/include/component
)

IF (CMAKE_SYSTEM_NAME MATCHES "Windows")
	ADD_DEFINITIONS(-D_CRT_SECURE_NO_WARNINGS)
ELSEIF (CMAKE_SYSTEM_NAME MATCHES "Linux")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall")
	add_definitions(-Wall)
ENDIF (CMAKE_SYSTEM_NAME MATCHES "Windows")

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
SET(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)
LINK_DIRECTORIES(${PROJECT_BINARY_DIR}/lib)

# compile component
ADD_SUBDIRECTORY(component/infra)

# compile googletest
ADD_SUBDIRECTORY(test/googletest)

# uint test
#ADD_SUBDIRECTORY(test/unit_test)

# api test
ADD_SUBDIRECTORY(test/test_tool/test_dynlib) #动态库测试工程
ADD_SUBDIRECTORY(test/api_test)
