project(pointviewer)
cmake_minimum_required(VERSION 3.0)

# Ignore anything in COMPILE_DEFINITIONS_<CONFIG>
if(POLICY CMP0043)
	cmake_policy(SET CMP0043 NEW)
endif()

# Ensure that our build type is set
set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Project build type")
if("${CMAKE_BUILD_TYPE}" STREQUAL "")
	set(CMAKE_BUILD_TYPE "Release")
endif()

set(Qt_VERSION_RELEASE 5 CACHE STRING "Qt release version")
set(Qt_VERSION_MAJOR   7 CACHE STRING "Qt major version")
set(Qt_VERSION_MINOR   1 CACHE STRING "Qt minor version")
set(Qt_VERSION ${Qt_VERSION_RELEASE}.${Qt_VERSION_MAJOR}.${Qt_VERSION_MINOR})

# Vendor FIND scripts, used for vendors that don't have find scripts or that we
# needed to create our own version
set(VENDOR_DIR ${CMAKE_SOURCE_DIR}/vendor)

# Set up platform dependendant variables before calling any find_packages
include(Platform.cmake)
if(UNIX)
	set(CMAKE_CXX_FLAGS "-std=c++11")

	# Detect version of Ubuntu
	execute_process(
		COMMAND cat /etc/issue
		OUTPUT_VARIABLE UBUNTU_VERSION
	)
	string(REGEX MATCH "[0-9]+.[0-9]+" UBUNTU_VERSION ${UBUNTU_VERSION})

	message(STATUS "Building on Ubuntu ${UBUNTU_VERSION}")

	# YCM shows a Qt warning that recommends building with -fPIC
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")

	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
	if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
		set(ASAN_DESCRIPTION "Turn on AddressSanitizer. Only usable in Debug builds.")
		option(ENABLE_ASAN ${ASAN_DESCRIPTION} OFF)

		option(ENABLE_TSAN "Turn on ThreadSanitizer. Only usable in Debug builds." OFF)

		if (${ENABLE_ASAN} AND ${ENABLE_TSAN})
			message(WARNING "You have asked to enable both ASAN and TSAN. This is not possible. Defaulting to TSAN.")
			set(ENABLE_ASAN OFF CACHE BOOL ${ASAN_DESCRIPTION} FORCE)
		endif()

		# Set the compile flags appropriately:
		if(${ENABLE_ASAN})
			set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=address -fno-omit-frame-pointer")

			# gcc needs some extra flags:
			if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
				set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -static-libasan")
			endif()
		elseif(${ENABLE_TSAN})
			set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=thread")
		endif()

		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-error=unused-command-line-argument -Qunused-arguments")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-tautological-compare")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-redeclared-class-member")

	endif()

	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-reorder")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-local-typedefs")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unknown-pragmas")


	# Shortcuts..
	if ("${UBUNTU_VERSION}" STREQUAL "16.04")
		set(Boost_NAMESPACE "ntcboost" CACHE STRING "Which boost namespace to use")
		set(OPAL_BOOST_VERSION "1.60" CACHE STRING "Boost version")
		set(PLATFORM linux64_1604)

		# This only seems to be required on 16.04
		set(THREADS_LIB pthread)

		set(OPAL_GEOLIB_VERSION 1.46)
	else()
		set(Boost_NAMESPACE "boost" CACHE STRING "Which boost namespace to use")
		set(Boost_VERSION "1.56" CACHE STRING "Boost version")
		set(PLATFORM linux64)
	endif()

	# Guess at where Qt can be found.
	set(Qt_DIR /opt/Qt${Qt_VERSION}/${Qt_VERSION_RELEASE}.${Qt_VERSION_MAJOR}/gcc_64 CACHE PATH "Qt's gcc_64 directory")

	message(STATUS "CXX flags: ${CMAKE_CXX_FLAGS}")

	# Install directory
	SET(INSTALL_PREFIX "$ENV{HOME}/pointapp/" CACHE PATH "Base installation path")
	SET(CPACK_GENERATOR "STGZ")

else() # if WIN32

	# Automatically link against the main qt static lib
	if(POLICY CMP0020)
		cmake_policy(SET CMP0020 NEW)
	endif()

	set(PLATFORM win64)

	# Guess at where Qt can be found.
	set(Qt_DIR c:/Qt/Qt${Qt_VERSION}/${Qt_VERSION_RELEASE}.${Qt_VERSION_MAJOR}/msvc2013_64 CACHE PATH "Qt's msvc2013_64 directory")

	message(STATUS "CXX flags: ${CMAKE_CXX_FLAGS}")

	# Install directory
	SET(INSTALL_PREFIX "$ENV{USERPROFILE}/pointapp/" CACHE PATH "Base installation path (governs CPACK_INSTALL_PREFIX and CMAKE_INSTALL_PREFIX)")
	SET(CPACK_NSIS_INSTALL_ROOT ${CPACK_INSTALL_PREFIX})

	# The comments and spacing are apparently critical to all this working, don't ask me why
	set(CPACK_NSIS_EXTRA_INSTALL_COMMANDS "\\n")
	set(CPACK_PACKAGE_INSTALL_DIRECTORY "pointapp")
	set(CPACK_NSIS_EXECUTABLES_DIRECTORY "${BIN_INSTALL_DIR}")

endif()

# If we don't hard set this, it always defaults to "/usr/local".
# Note, CPACK_INSTALL_PREFIX will default to CMAKE_INSTALL_PREFIX
set(CMAKE_INSTALL_PREFIX ${INSTALL_PREFIX})
message(STATUS "Install prefix (set with INSTALL_PREFIX) set to ${CMAKE_INSTALL_PREFIX}")

message(STATUS "Using Qt${Qt_VERSION_RELEASE} found at ${Qt_DIR}")

# Specifiy where Qt5Core can be found, all the other Qt modules can be
# found as components to this.
set(Qt${Qt_VERSION_RELEASE}_DIR "${Qt_DIR}/lib/cmake/Qt${Qt_VERSION_RELEASE}")


if (WIN32)
	# Projects should populate this with the external link directories they
	# require (their rpath) for windows
	set(EXTERNAL_LINK_DIRS)
	list(APPEND EXTERNAL_LINK_DIRS ${Qt_DIR}/bin)

	add_definitions(-DBOOST_ALL_DYN_LINK)
endif()


# Install directives used in the child cmake files (the rest of the install stuff is done at the bottom of the file)
set(INSTALL_BIN_REL_DIR "bin")
set(INSTALL_BIN_DIR     "\${CMAKE_INSTALL_PREFIX}/${INSTALL_BIN_REL_DIR}")

set(MY_COMPONENT_APP "app")
set(CPACK_COMPONENT_${MY_COMPONENT_APP}_DISPLAY_NAME "Points Viewer")

set(RUN_PATH)
list(APPEND RUN_PATH ${EXTERNAL_LINK_DIRS})
if (NOT WIN32)
	list(APPEND RUN_PATH ${Qt_DIR}/lib)
endif()
# /install stuff


# Build our actual app
add_subdirectory(app)


if (WIN32)
	include(${CMAKE_SOURCE_DIR}/Win32LaunchScripts.cmake)

	# Create a batch file to set the path to all the libs we need
	get_property(tmp TARGET app PROPERTY IMPORTED_LINK_INTERFACE_LIBRARIES)

	message(STATUS "Outputting batch file to set rpath: Set_Path.bat")
	file(WRITE ${PROJECT_BINARY_DIR}/Set_Path_${CMAKE_BUILD_TYPE}.bat
"
@echo off

REM EnableDelayedExpansion: http://ss64.com/nt/delayedexpansion.html
SETLOCAL EnableDelayedExpansion

REM Check if the project paths is already defined.  If so, remove
REM changes first, else save env before change.

if DEFINED MATT_PROJECT_PATH (
	set PATH=!MATT_PATH_ORIGINAL!
) ELSE (
	set MATT_PATH_ORIGINAL=!PATH!
	set MATT_PROJECT_PATH=${CMAKE_BINARY_DIR}
	set MATT_PROJECT_PATH=!MATT_PROJECT_PATH!
)

ECHO =======================================================
call ECHO New Project Path : %MATT_PROJECT_PATH%
call ECHO New Project Type : ${CMAKE_BUILD_TYPE}
ECHO =======================================================

set MATT_PATH_=${EXTERNAL_LINK_DIRS};!PATH!

REM Use & to run the commands in parallel, without that they can't access
REM access the _ variables.  It's as if they're both in and out of the
REM local scope.

ENDLOCAL & SET PATH=%MATT_PATH_% & SET MATT_PATH_ORIGINAL=%MATT_PATH_ORIGINAL% & SET MATT_PROJECT_PATH=%MATT_PROJECT_PATH%

"
	) # /file

	# Generate a run script that'll load the set path batch file and the solution
	GENERATE_VISUAL_STUDIO_LAUNCH_SCRIPTS(pointviewer)
endif()


################################################################################
# CPACK definitions
################################################################################
set(CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS_SKIP ON)
include(InstallRequiredSystemLibraries)

# Get the Version Numbers
include(VersionNumber.cmake)

set(CPACK_PACKAGE_NAME "pointapp")
set(CPACK_PACKAGE_VENDOR "Matt App")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Point Viewer")

set(CPACK_PACKAGE_VERSION "${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}")

set(CPACK_PACKAGE_CONTACT "matthew.g.russell@gmail.com")

# This must always be last!
include(CPack)

# vim: ts=4 : sw=4 : sts=0 : noet : ft=cmake : ffs=unix,dos :
