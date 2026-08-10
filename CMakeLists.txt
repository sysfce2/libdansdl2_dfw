cmake_minimum_required(VERSION 3.5.1)
project(dfw)

option(BUILD_STATIC "generate static library" OFF)
option(BUILD_SHARED "generate shared library" ON)
option(BUILD_DEBUG "generate shared debug library" OFF)
option(BUILD_TESTS "Build test code" OFF)

#library version
set(MAJOR_VERSION 1)
set(MINOR_VERSION 3)
set(PATCH_VERSION 0)

if(${BUILD_DEBUG})

	set(RELEASE_VERSION "debug")

else()

	set(RELEASE_VERSION "prod")
endif()

if(${CMAKE_VERSION} VERSION_LESS "3.22.0")

	add_definitions(-DMAJOR_VERSION=${MAJOR_VERSION})
	add_definitions(-DMINOR_VERSION=${MINOR_VERSION})
	add_definitions(-DPATCH_VERSION=${PATCH_VERSION})
	add_definitions(-DRELEASE_VERSION=\"${RELEASE_VERSION}\")
else()

	add_compile_definitions(MAJOR_VERSION=${MAJOR_VERSION})
	add_compile_definitions(MINOR_VERSION=${MINOR_VERSION})
	add_compile_definitions(PATCH_VERSION=${PATCH_VERSION})
	add_compile_definitions(RELEASE_VERSION=\"${RELEASE_VERSION}\")
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_compile_options(-Wall -Wextra -Wundef -Wcast-align -Wwrite-strings -Wlogical-op -Wmissing-declarations -Wredundant-decls -Wshadow -Woverloaded-virtual -Wno-deprecated -pedantic -fno-rtti)

#Add the include directories
include_directories("include")

set(SOURCE "")
add_subdirectory("${PROJECT_SOURCE_DIR}/lib")

#library type and filenames.
if(${BUILD_DEBUG})

	set(CMAKE_BUILD_TYPE Debug)
	set(LIB_FILENAME "dfw_debug")
else()

	set(CMAKE_BUILD_TYPE Release)
	set(LIB_FILENAME "dfw")
endif()

if(${BUILD_STATIC})

	add_library(dfw_static STATIC ${SOURCE})
	set_target_properties(dfw_static PROPERTIES OUTPUT_NAME ${LIB_FILENAME})
	target_compile_definitions(dfw_static PUBLIC "-DLIB_VERSION=\"static\"")
	install(TARGETS dfw_static DESTINATION lib)

	message("will build ${MAJOR_VERSION}.${MINOR_VERSION}.${PATCH_VERSION}-${RELEASE_VERSION}-static")
endif()

if(${BUILD_SHARED})

	add_library(dfw_shared SHARED ${SOURCE})
	set_target_properties(dfw_shared PROPERTIES OUTPUT_NAME ${LIB_FILENAME})
	target_compile_definitions(dfw_shared PUBLIC "-DLIB_VERSION=\"shared\"")
	install(TARGETS dfw_shared DESTINATION lib)

	message("will build ${MAJOR_VERSION}.${MINOR_VERSION}.${PATCH_VERSION}-${RELEASE_VERSION}-shared")
endif()

IF(WIN32)

	include_directories("${CMAKE_CURRENT_LIST_DIR}/../log/include/")
	include_directories("${CMAKE_CURRENT_LIST_DIR}/../libdansdl2/include/")
	include_directories("${CMAKE_CURRENT_LIST_DIR}/../ldtools/include/")
	include_directories("${CMAKE_CURRENT_LIST_DIR}/../tools/include/")

	if(${BUILD_STATIC})
		target_compile_definitions(dfw_static PUBLIC -DWINBUILD)
	endif()

	if(${BUILD_SHARED}) 
		target_compile_definitions(dfw_shared PUBLIC -DWINBUILD)

		set(IMPORTED_LIB_POSTFIX "")
		if(${BUILD_DEBUG})

			set(IMPORTED_LIB_POSTFIX "_debug")
		endif()

		#in windows we cannot allow unresolved symbols...
		add_library(lm SHARED IMPORTED)
		set_target_properties(lm PROPERTIES 
			IMPORTED_IMPLIB "../../log/build/liblm${IMPORTED_LIB_POSTFIX}.dll.a"
			IMPORTED_LOCATION "../../log/build/liblm${IMPORTED_LIB_POSTFIX}.dll"
		)

		add_library(dansdl2 SHARED IMPORTED)
		set_target_properties(dansdl2 PROPERTIES 
			IMPORTED_IMPLIB "../../libdansdl2/build/libdansdl2${IMPORTED_LIB_POSTFIX}.dll.a"
			IMPORTED_LOCATION "../../libdansdl2/build/libdansdl2${IMPORTED_LIB_POSTFIX}.dll"
		)

		add_library(ldtools SHARED IMPORTED)
		set_target_properties(ldtools PROPERTIES 
			IMPORTED_IMPLIB "../../ldtools/build/libldtools${IMPORTED_LIB_POSTFIX}.dll.a"
			IMPORTED_LOCATION "../../ldtools/build/libldtools${IMPORTED_LIB_POSTFIX}.dll"
		)

		add_library(tools SHARED IMPORTED)
		set_target_properties(tools PROPERTIES 
			IMPORTED_IMPLIB "../../tools/build/libtools${IMPORTED_LIB_POSTFIX}.dll.a"
			IMPORTED_LOCATION "../../tools/build/libtools${IMPORTED_LIB_POSTFIX}.dll"
		)

		target_link_libraries(dfw_shared lm dansdl2 ldtools tools SDL2 SDL2_ttf SDL2_mixer SDL2_image glu32 opengl32 stdc++fs)
	endif()
endif()

install(DIRECTORY include/ DESTINATION include)
