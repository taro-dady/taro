
PROJECT (test_dynlib)

file(GLOB SRC_CPP src/*)
file(GLOB SRC_H   include/*)

INCLUDE_DIRECTORIES(
	include
)

SET(CMAKE_CXX_STANDARD 11)

# shared library
ADD_LIBRARY(test_dynlib SHARED ${SRC_CPP} ${SRC_H})
