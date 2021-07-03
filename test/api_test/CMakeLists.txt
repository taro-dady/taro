
PROJECT (sit)

INCLUDE_DIRECTORIES(
	include
)

IF (CMAKE_SYSTEM_NAME MATCHES "Windows")
	ADD_DEFINITIONS(-D_WINSOCK_DEPRECATED_NO_WARNINGS -DWIN32_LEAN_AND_MEAN)
ELSEIF (CMAKE_SYSTEM_NAME MATCHES "Linux")
ENDIF (CMAKE_SYSTEM_NAME MATCHES "Windows")

file(GLOB_RECURSE SRC_CPP src/*)

ADD_EXECUTABLE(sit ${SRC_CPP})

IF (CMAKE_SYSTEM_NAME MATCHES "Windows")
TARGET_LINK_LIBRARIES( sit flow_ctrl authority encrypt config task startup net data_base log basic gtest )
ELSEIF (CMAKE_SYSTEM_NAME MATCHES "Linux")
if(LINK_THREAD MATCHES "FALSE")
    message("not link thread")
    TARGET_LINK_LIBRARIES( sit flow_ctrl authority encrypt config task startup net data_base log basic gtest dl )
else()
	message("link thread")
    TARGET_LINK_LIBRARIES( sit flow_ctrl authority encrypt config task startup net data_base log basic gtest pthread dl )
endif()

ENDIF (CMAKE_SYSTEM_NAME MATCHES "Windows")
