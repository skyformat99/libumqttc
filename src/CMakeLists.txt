
add_definitions(-O -Wall -Werror --std=gnu99 -D_GNU_SOURCE -Wno-misleading-indentation)

# The version number.
set(UMQTTC_VERSION_MAJOR 0)
set(UMQTTC_VERSION_MINOR 0)
set(UMQTTC_VERSION_PATCH 1)

# Check the third party Libraries
find_package(Libubox REQUIRED)

include_directories(${CMAKE_CURRENT_BINARY_DIR} ${LIBUBOX_INCLUDE_DIR})

set(EXTRA_LIBS ${LIBUBOX_LIBRARY} dl)
set(SOURCE_FILES umqttc.c log.c utils.c)

set(UMQTTC_SSL_SUPPORT_CONFIG 1)
option(UMQTTC_SSL_SUPPORT "SSL support" ON)

if(NOT UMQTTC_SSL_SUPPORT)
    set(UMQTTC_SSL_SUPPORT_CONFIG 0)
endif()

add_library(umqttc SHARED ${SOURCE_FILES})
set_target_properties(umqttc PROPERTIES VERSION ${UMQTTC_VERSION_MAJOR}.${UMQTTC_VERSION_MINOR}.${UMQTTC_VERSION_PATCH})
target_link_libraries(umqttc ${EXTRA_LIBS})

# configure a header file to pass some of the CMake settings to the source code
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.h.in ${CMAKE_CURRENT_BINARY_DIR}/config.h)

install(
    FILES
	    ${CMAKE_CURRENT_BINARY_DIR}/config.h
	    ${CMAKE_CURRENT_SOURCE_DIR}/umqttc.h
	    ${CMAKE_CURRENT_SOURCE_DIR}/log.h
    DESTINATION
    	include/umqttc
)

install(
    TARGETS umqttc LIBRARY
    DESTINATION lib
)

message("")
message(STATUS "UMQTTC_VERSION: ${UMQTTC_VERSION_MAJOR}.${UMQTTC_VERSION_MINOR}.${UMQTTC_VERSION_PATCH}")
message(STATUS "UMQTTC_SSL_SUPPORT: ${UMQTTC_SSL_SUPPORT}")
message("")
