cmake_minimum_required(VERSION 3.4.1)

project(DataReporter)

#1.-------Add source directory-------
set( DATA_REPORTER_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/core/base
    ${CMAKE_CURRENT_SOURCE_DIR}/core/reporter
    ${CMAKE_CURRENT_SOURCE_DIR}/core/thread)

if (APPLE)
    list(APPEND
        DATA_REPORTER_INCLUDE_DIRS
        ${CMAKE_CURRENT_SOURCE_DIR}/call_interface/ios
        ${CMAKE_CURRENT_SOURCE_DIR}/platform/ios)
else ()
    list(APPEND
        DATA_REPORTER_INCLUDE_DIRS
        ${CMAKE_CURRENT_SOURCE_DIR}/call_interface/android
        ${CMAKE_CURRENT_SOURCE_DIR}/platform/android
		${CMAKE_CURRENT_SOURCE_DIR}/platform/android/util)
endif ()


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
        if (APPLE)
            source_group(TREE ${tempDir} PREFIX "Header Files/${SOURCE_GROUP_NAME}" FILES ${tempFile} )
        else ()
            source_group("${SOURCE_GROUP_NAME}" FILES ${tempFile} )
        endif ()
    endforeach ()
endforeach ()


#4.-------Add source files-------
set(SOURCE_FILES "")
foreach (tempDir ${DATA_REPORTER_INCLUDE_DIRS})
    set(SUB_SOURCE_FILES "")
    file(GLOB_RECURSE SUB_SOURCE_FILES
              "${tempDir}/*.c"
              "${tempDir}/*.cpp")

    if (APPLE)
        file(GLOB SUB_SOURCE_FILES
                  "${tempDir}/*.c"
                  "${tempDir}/*.cpp"
                  "${tempDir}/*.m"
                  "${tempDir}/*.mm")
    endif ()
    foreach (tempFile ${SUB_SOURCE_FILES})
        list(APPEND SOURCE_FILES ${tempFile})

        string(REPLACE
              ${CMAKE_CURRENT_SOURCE_DIR}
              ""
              SOURCE_GROUP_NAME
              ${tempDir})
        if (APPLE)
            source_group(TREE ${tempDir} PREFIX "Source Files/${SOURCE_GROUP_NAME}" FILES ${tempFile} )
        else ()
            source_group("${SOURCE_GROUP_NAME}" FILES ${tempFile} )
        endif ()
    endforeach ()
endforeach ()


#-------5.Add iOS framework-------
set(EXTRA_FRAMEWORKS "")
if (APPLE)
    find_library(IOS_Foundation Foundation)
    list(APPEND EXTRA_FRAMEWORKS ${IOS_Foundation})
endif ()


#-------6.Configure library-------
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1y")

if (APPLE)
    add_library(data-reporter
                STATIC
                ${SOURCE_FILES}
                ${HEADER_FILES})
else ()
    add_library(data-reporter
                SHARED
                ${SOURCE_FILES}
                ${HEADER_FILES})
endif ()

target_link_libraries(data-reporter z ${EXTRA_FRAMEWORKS})
