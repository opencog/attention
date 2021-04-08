#
# Master Attention Allocation CMake file.
#
# General organization:
# -- check for different compilers, OS'es
# -- search for various required & optional libraries/tools
# -- decide what to build based on above results.
# -- configure various config files.
# -- print pretty summary
#

# We need at least version 3.13 to get TARGET_LINK_DIRECTORIES
# Argh. circleci does not have that version.
# Apparently, version 3.5 is what Ubuntu Xenial has, so ask for that.
MESSAGE("Found CMake version ${CMAKE_VERSION}")
CMAKE_MINIMUM_REQUIRED(VERSION 3.5)

# Well, that's weird. We need `CMP0037 OLD` as otherwise, cmake
# version 3.10.2 complains :
#   The target name "test" is reserved or not valid for certain CMake features,
#      such as generator expressions, and may result in undefined behavior.
# However, cmake version 3.13.4 does not complain about the "test"
# target, and everything just works fine ...
IF (NOT CMAKE_VERSION VERSION_GREATER 3.13)
	CMAKE_POLICY(SET CMP0037 OLD)
ENDIF (NOT CMAKE_VERSION VERSION_GREATER 3.13)

PROJECT(attention)

# ----------------------------------------------------------
# User-modifiable options. Feel free to change these!
#
# uncomment to be in Release mode [default]
# SET(CMAKE_BUILD_TYPE Release)

# uncomment to build in debug mode
# SET(CMAKE_BUILD_TYPE Debug)

# uncomment to be in coverage testing mode
# SET(CMAKE_BUILD_TYPE Coverage)

# uncomment to build in profile mode
# SET(CMAKE_BUILD_TYPE Profile)

# uncomment to build in release mode with debug information
# SET(CMAKE_BUILD_TYPE RelWithDebInfo)

# default build type
IF (CMAKE_BUILD_TYPE STREQUAL "")
	SET(CMAKE_BUILD_TYPE Release)
ENDIF (CMAKE_BUILD_TYPE STREQUAL "")

MESSAGE(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

ADD_DEFINITIONS(-DPROJECT_SOURCE_DIR="${CMAKE_SOURCE_DIR}"
                -DPROJECT_BINARY_DIR="${CMAKE_BINARY_DIR}")

# ===============================================================
# Check for existance of various required, optional packages.
# Listed in alphabetical order, more or less.
# CogUtil must come first, because it supplies various FindXXX macros.

# Add the 'lib' dir to cmake's module search path
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/lib/")

# Cogutil
FIND_PACKAGE(CogUtil 2.0.1 CONFIG REQUIRED)
IF (COGUTIL_FOUND)
	MESSAGE(STATUS "CogUtil found.")
	ADD_DEFINITIONS(-DHAVE_COGUTIL)
	SET(HAVE_COGUTIL 1)
ELSE (COGUTIL_FOUND)
	MESSAGE(FATAL_ERROR "CogUtil missing: it is needed!")
ENDIF (COGUTIL_FOUND)

# add the 'cmake' directory from cogutil to search path
list(APPEND CMAKE_MODULE_PATH  ${COGUTIL_DATA_DIR}/cmake)

include(OpenCogGccOptions)
include(OpenCogLibOptions)
include(OpenCogInstallOptions)
include(Summary)

# ===================================================================
# Check for existance of various required, optional packages.

# AtomSpace
FIND_PACKAGE(AtomSpace 5.0.3 CONFIG REQUIRED)
IF (ATOMSPACE_FOUND)
	MESSAGE(STATUS "AtomSpace found.")
	ADD_DEFINITIONS(-DHAVE_ATOMSPACE)
	SET(HAVE_ATOMSPACE 1)
ELSE (ATOMSPACE_FOUND)
	MESSAGE(FATAL_ERROR "AtomSpace missing: it is needed!")
ENDIF (ATOMSPACE_FOUND)

# ===================================================================
# Check for existance of various required, optional packages.

# CogServer
FIND_PACKAGE(CogServer)
IF (COGSERVER_FOUND)
	MESSAGE(STATUS "CogServer found.")
	ADD_DEFINITIONS(-DHAVE_COGSERVER)
	SET(HAVE_COGSERVER 1)
ELSE (COGSERVER_FOUND)
	MESSAGE(FATAL_ERROR "CogServer missing: it is needed!")
ENDIF (COGSERVER_FOUND)

# ----------------------------------------------------------
# Check for boost. We need dynamic-linked, threaded libs by default.
SET(Boost_USE_STATIC_LIBS OFF)
SET(Boost_USE_MULTITHREADED ON)
SET(MIN_BOOST 1.46)

# Required boost packages
FIND_PACKAGE(Boost ${MIN_BOOST} REQUIRED COMPONENTS filesystem system)

IF(Boost_FOUND)
	SET(Boost_FOUND_SAVE 1)
ELSE(Boost_FOUND)
	MESSAGE(FATAL_ERROR "Boost ${MIN_BOOST} or newer is needed to build OpenCog!")
ENDIF(Boost_FOUND)

# Opencog won't compile with Boost 1.51, some kind of conflict with
# hash functions, see github bugs 1 and 36
IF(105100 EQUAL ${Boost_VERSION})
	MESSAGE(FATAL_ERROR "Boost version 1.51 will not work with OpenCog.  Please use a different version.")
ENDIF(105100 EQUAL ${Boost_VERSION})
MESSAGE(STATUS "Boost version ${Boost_VERSION} found.")

# Optional boost packages; can build without these.
# Is this actually needed?
FIND_PACKAGE(Boost ${MIN_BOOST} COMPONENTS python QUIET)

# Arghhh. Except cmake is treating above as required, not optional. #$%**&
IF(Boost_FOUND_SAVE)
	SET(Boost_FOUND 1)
ENDIF(Boost_FOUND_SAVE)

# ----------------------------------------------------------
# Needed for unit tests

FIND_PACKAGE(Cxxtest)
IF (NOT CXXTEST_FOUND)
	MESSAGE(STATUS "CxxTest missing: needed for unit tests.")
ENDIF (NOT CXXTEST_FOUND)

# ----------------------------------------------------------
# This is required for Guile Python and Cython

include(OpenCogFindGuile)
include(OpenCogFindPython)

# ----------------------------------------------------------
# Optional, currently needed only to hush up DRD in util/Logger.cc
FIND_PACKAGE(VALGRIND)
IF (VALGRIND_FOUND)
	MESSAGE(STATUS "VALGRIND was found.")
	IF (VALGRIND_INCLUDE_DIR)
		MESSAGE(STATUS "VALGRIND devel headers found.")
		ADD_DEFINITIONS(-DHAVE_VALGRIND)
	ELSE (VALGRIND_INCLUDE_DIR)
		MESSAGE(STATUS "VALGRIND devel headers NOT FOUND: needed for thread debugging.")
	ENDIF (VALGRIND_INCLUDE_DIR)
ELSE (VALGRIND_FOUND)
	MESSAGE(STATUS "VALGRIND missing: needed for thread debugging.")
ENDIF (VALGRIND_FOUND)

# ===================================================================
# Include configuration.

# Set default include paths.
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}
	${Boost_INCLUDE_DIRS}
	${COGUTIL_INCLUDE_DIR}
	${ATOMSPACE_INCLUDE_DIR})

# Macros that define how atom types get declared.
IF (NOT DEFINED ATOMSPACE_DATA_DIR)
	SET (ATOMSPACE_DATA_DIR "${COGUTIL_DATA_DIR}")
ENDIF (NOT DEFINED ATOMSPACE_DATA_DIR)

INCLUDE("${ATOMSPACE_DATA_DIR}/cmake/OpenCogMacros.cmake")
INCLUDE("${ATOMSPACE_DATA_DIR}/cmake/OpenCogGuile.cmake")
INCLUDE("${ATOMSPACE_DATA_DIR}/cmake/OpenCogCython.cmake")

# ==========================================================
# Decide what to build, based on the packages found.

IF(HAVE_COGSERVER AND HAVE_ATOMSPACE AND HAVE_COGUTIL)
	SET(HAVE_ATTENTION 1)
ENDIF(HAVE_COGSERVER AND HAVE_ATOMSPACE AND HAVE_COGUTIL)

ADD_SUBDIRECTORY(lib)
ADD_SUBDIRECTORY(opencog)
ADD_SUBDIRECTORY(examples EXCLUDE_FROM_ALL)

ADD_CUSTOM_TARGET(uninstall
	COMMAND bash -c "cat install_manifest.txt | xargs rm -f"
	VERBATIM
	COMMENT "Uninstalling AttentionBank ..."
)

IF (CXXTEST_FOUND)
	ADD_CUSTOM_TARGET(tests)
	ADD_SUBDIRECTORY(tests EXCLUDE_FROM_ALL)
	IF (CMAKE_BUILD_TYPE STREQUAL "Coverage")
		# doing coverage stuff while running tests if this is the Coverage build
		ADD_CUSTOM_TARGET(test
			# TODO lcov should be found by cmake first
			# TODO set it up so that we can pick to run coverage per test, or
			# combined across all tests (the latter is MUCH faster). Use a define?
			# There is coverage specific stuff in AddCxxTest.cmake now...
			# -
			# Depends on cogserver because RESTFulTest needs it
			# and cmake has no way to add non-test dependencies to tests
			DEPENDS tests cogserver
			WORKING_DIRECTORY tests
			COMMAND ${CMAKE_CTEST_COMMAND} --force-new-ctest-process --output-on-failure $(ARGS)

			# This script combines the coverage analysis of each test,
			# then creates html in tests/lcov
			# Note: this should now be run separately...
			#COMMAND ${PROJECT_SOURCE_DIR}/scripts/combine_lcov.sh
			COMMENT "Running tests with coverage..."
		)
	ELSE (CMAKE_BUILD_TYPE STREQUAL "Coverage")
		# If this is a build with coverage enabled then test normally
		ADD_CUSTOM_TARGET(test
			DEPENDS tests
			WORKING_DIRECTORY tests
			COMMAND ${CMAKE_CTEST_COMMAND} --force-new-ctest-process --output-on-failure $(ARGS)
			COMMENT "Running tests..."
		)
	ENDIF (CMAKE_BUILD_TYPE STREQUAL "Coverage")
ENDIF (CXXTEST_FOUND)

# ===================================================================
# Packaging
## Architecture the package is for.
## TODO: Will give error on non debian distros, fix it.
EXECUTE_PROCESS(COMMAND  dpkg --print-architecture
	OUTPUT_VARIABLE PACKAGE_ARCHITECTURE
	OUTPUT_STRIP_TRAILING_WHITESPACE)
STRING(TIMESTAMP UTC_DATE %Y%m%d UTC)
# If 'sudo make install' is run before 'make package', then install_manifest.txt
# will be owned by root. Creating the file during configuration stage ensures
# that it is owned by the builder thus avoiding 'Permission denied' error when
# packaging.
FILE(WRITE "${PROJECT_BINARY_DIR}/install_manifest.txt")
## It doesn't have a header-file declaring the version similar to cogutil and
## atomspace.
SET(SEMANTIC_VERSION 0.1.4)

## Cpack configuration
SET(CPACK_GENERATOR "DEB")
SET(CPACK_PACKAGE_CONTACT "opencog@googlegroups.com")
SET(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/README.md")
SET(CPACK_PACKAGE_DIRECTORY "${CMAKE_BINARY_DIR}/packages")
SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "The Open Cognition Framework")
SET(CPACK_PACKAGE_NAME "attention-dev")
SET(CPACK_PACKAGE_VENDOR "opencog.org")
SET(CPACK_PACKAGE_VERSION "${SEMANTIC_VERSION}-${UTC_DATE}")
SET(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
SET(CPACK_PACKAGE_FILE_NAME
	"${CPACK_PACKAGE_NAME}_${CPACK_PACKAGE_VERSION}_${PACKAGE_ARCHITECTURE}")
SET(CPACK_PACKAGING_INSTALL_PREFIX "/usr/local")

## Debian specific configurations
SET(DEPENDENCY_LIST
	"guile-2.2-dev (>= 2.2.2)"
	"python3-dev (>= 3.6.7)"
	"libboost-dev (>= ${MIN_BOOST})"
	"libstdc++6 (>= 4.7)"
	"libcogutil-dev (>= 2.0.2)"
	"atomspace-dev (>= 5.0.3)"
)

STRING(REPLACE ";" ", " MAIN_DEPENDENCIES "${DEPENDENCY_LIST}")
SET(CPACK_DEBIAN_PACKAGE_DEPENDS "${MAIN_DEPENDENCIES}")
SET(CPACK_DEBIAN_PACKAGE_SECTION "libdevel")
SET(CPACK_DEBIAN_PACKAGE_HOMEPAGE "http://opencog.org")
INCLUDE(CPack)

# ===================================================================
# Documentation.
FIND_PACKAGE(Doxygen)
# ADD_SUBDIRECTORY(doc EXCLUDE_FROM_ALL)

# ===================================================================
# Show a summary of what we found, what we will do.

SUMMARY_ADD("Attention" "Agents for attention allocation dynamics" HAVE_ATTENTION)
SUMMARY_ADD("Cython bindings" "Cython (python) bindings" HAVE_CYTHON)
SUMMARY_ADD("Doxygen" "Code documentation" DOXYGEN_FOUND)
SUMMARY_ADD("Python tests" "Python bindings nose tests" HAVE_NOSETESTS)
SUMMARY_ADD("Unit tests" "Unit tests" CXXTEST_FOUND)

SUMMARY_SHOW()
