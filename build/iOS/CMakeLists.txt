# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.4.1)

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.
#


#if (CMAKE_BUILD_TYPE MATCHES "Debug")
#option(MEMORY_CHECK         "Set to switch to build use SSE"  ON)
#add_definitions(-D MEMORY_CHECK)
#endif()

get_filename_component(CMAKE_SOURCE_DIR
         "../../src"
         ABSOLUTE)

get_filename_component(CMAKE_CURRENT_SOURCE_DIR
         "../../src"
         ABSOLUTE)
		 
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

MESSAGE( STATUS "src_dir = ${CMAKE_SOURCE_DIR}.")
MESSAGE( STATUS "current_src_dir = ${CMAKE_CURRENT_SOURCE_DIR}.")


file(GLOB_RECURSE HEAD_FILES ${CMAKE_SOURCE_DIR}/*.h ${CMAKE_SOURCE_DIR}/*.hpp)
foreach(PATH  IN LISTS HEAD_FILES)
		string (FIND ${PATH} "/android/" _index_android)
		string (FIND ${PATH} "/libleaktracer/" _index_libleaktracer)

		if (${_index_android} EQUAL -1
				)

			if(NOT MEMORY_CHECK)
				if(${_index_libleaktracer} GREATER 0)
					continue()
					endif()
				endif()
			#包含路径
			get_filename_component(HEAD_DIR ${PATH} DIRECTORY)
			message(STATUS "include_dir:${HEAD_DIR}\n")
			include_directories(${HEAD_DIR})
		endif()
endforeach()

include_directories(${CMAKE_SOURCE_DIR}/typesetting/engine/render/html/core/third-party/skia/android)

file(GLOB_RECURSE SOURCES ${CMAKE_SOURCE_DIR}/*.cpp ${CMAKE_SOURCE_DIR}/*.c ${CMAKE_SOURCE_DIR}/*.cc ${CMAKE_SOURCE_DIR}/*.mm)
foreach(file IN LISTS SOURCES)
    	string (FIND ${file} "/android/" _index_android)
		string (FIND ${file} "/libleaktracer/" _index_libleaktracer)

		#message(STATUS "file_is:${file}\n")

    	if (${_index_android} EQUAL -1
				)

			if(NOT MEMORY_CHECK)
				if(${_index_libleaktracer} GREATER 0)
					continue()
				endif()
			endif()
				list(APPEND SOURCE_FILES "${file};")
				message(STATUS "src_files:${SOURCE_FILES}\n")
        endif()
endforeach()

source_group(TREE ${CMAKE_SOURCE_DIR} FILES  ${SOURCE_FILES} )

add_definitions(-DHAVE_EXPAT_CONFIG_H )

add_library( # Sets the name of the library.
             data-reporter
             # Sets the library as a shared library.
             STATIC
             ${SOURCE_FILES} )

# Searches for a specified prebuilt library and stores the path as a
# variable. Because CMake includes system libraries in the search path by
# default, you only need to specify the name of the public NDK library
# you want to add. CMake verifies that the library exists before
# completing its build.

#find_library( # Sets the name of the path variable.
#              log-lib
              # Specifies the name of the NDK library that
              # you want CMake to locate.
#              log )

# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.


target_link_libraries( # Specifies the target library.
                       data-reporter
                       #${log-lib}
		)