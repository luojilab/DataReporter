cmake_minimum_required(VERSION 3.4.1)

project(DataReporter)

get_filename_component(CMAKE_SOURCE_DIR
         "../../../src"
         ABSOLUTE)

get_filename_component(CMAKE_CURRENT_SOURCE_DIR
         "../../../src"
         ABSOLUTE)

#1.-------Add source directory-------
set( DATA_REPORTER_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/core/base
    ${CMAKE_CURRENT_SOURCE_DIR}/core/crypto
    ${CMAKE_CURRENT_SOURCE_DIR}/core/reporter
    ${CMAKE_CURRENT_SOURCE_DIR}/core/thread)

list(APPEND
    DATA_REPORTER_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/call_interface/android
    ${CMAKE_CURRENT_SOURCE_DIR}/platform/android
	${CMAKE_CURRENT_SOURCE_DIR}/platform/android/util)



#2.-------Add include directory-------
include_directories( BEFORE
    ${DATA_REPORTER_INCLUDE_DIRS})


#3.-------Add header files-------
set(HEADER_FILES "")
foreach (tempDir ${DATA_REPORTER_INCLUDE_DIRS})
    file(GLOB_RECURSE SUB_HEADER_FILES "${tempDir}/*.h")
    foreach (tempFile ${SUB_HEADER_FILES})
        list(APPEND HEADER_FILES ${tempFile})
        string(REPLACE
               ${CMAKE_CURRENT_SOURCE_DIR}
               ""
               SOURCE_GROUP_NAME
               ${tempDir})
        source_group("${SOURCE_GROUP_NAME}" FILES ${tempFile} )
    endforeach ()
endforeach ()


#4.-------Add source files-------
set(SOURCE_FILES "")
foreach (tempDir ${DATA_REPORTER_INCLUDE_DIRS})
    set(SUB_SOURCE_FILES "")
    file(GLOB_RECURSE SUB_SOURCE_FILES
              "${tempDir}/*.c"
              "${tempDir}/*.cpp")

    foreach (tempFile ${SUB_SOURCE_FILES})
        list(APPEND SOURCE_FILES ${tempFile})

        string(REPLACE
              ${CMAKE_CURRENT_SOURCE_DIR}
              ""
              SOURCE_GROUP_NAME
              ${tempDir})
        source_group("${SOURCE_GROUP_NAME}" FILES ${tempFile} )
    endforeach ()
endforeach ()




#-------6.Configure library-------
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

add_library(data-reporter 
            SHARED
            ${SOURCE_FILES}
            ${HEADER_FILES})

#
#
target_link_libraries(data-reporter z ${EXTRA_FRAMEWORKS})
