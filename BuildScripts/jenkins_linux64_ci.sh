#!/bin/bash

# This script performs the linux64 build.

# Tell bash to exit on any error. This way, if the
# build fails, cppcheck won't run:
set -e

declare -r WORKSPACE=${1}

# define build and installation directories
declare -r BUILD_DIR=${WORKSPACE}/build_dir
declare -r INSTALL_DIR=${WORKSPACE}/install_dir

# Note, here we do not delete the build_dir each time,
# since we just want to build and integrate the new commits,
# instead of a whole clean build each time.
mkdir -p ${BUILD_DIR}
cd ${BUILD_DIR}

declare -r Qt_VERSION_RELEASE=5
declare -r Qt_VERSION_MAJOR=8
declare -r Qt_VERSION_MINOR=0

declare -r BUILD_TYPE=Release
declare -r Qt_DIR=/opt/Qt${Qt_VERSION_RELEASE}.${Qt_VERSION_MAJOR}.${Qt_VERSION_MINOR}/${Qt_VERSION_RELEASE}.${Qt_VERSION_MAJOR}/gcc_64
declare -r SOURCE_DIR=..

# Run CMake
cmake                                        \
	-G"Ninja"                                  \
	-DQt_VERSION_RELEASE=${Qt_VERSION_RELEASE} \
	-DQt_VERSION_MAJOR=${Qt_VERSION_MAJOR}     \
	-DQt_VERSION_MINOR=${Qt_VERSION_MINOR}     \
	-DQt_DIR=${Qt_DIR}                         \
	-DCMAKE_BUILD_TYPE=${BUILD_TYPE}           \
	${SOURCE_DIR}                              \
	&& ninja


# Run cppcheck (static code analysis) on the source: cppcheck must be run from
# the top level workspace directory so the paths will be correct:
# Derived the include paths by using the `jq` command on compile_commands.json, i.e.
#
# Then cleaned them up here in vim with: :'<,'>s/\v"(.*)"/\t-I\1 \\
#
# An alternative (cppcheck version dependent, it's a new feature) is to include
# the project file with --project for either the compile_commands.json or VS
# solution file.
#
# Recall: "-j <n>" prevents chekcing for unused functions.
cd ${WORKSPACE}/..
cppcheck -j 4                         \
	--suppressions-list=${WORKSPACE}/BuildScripts/suppressions.txt \
	--enable=all                        \
	--inconclusive                      \
	--xml --xml-version=2               \
	-I${WORKSPACE}/app/include                                          \
	-I${Qt_DIR}/include                                                 \
	-I${Qt_DIR}/include/Qt3DCore                                        \
	-I${Qt_DIR}/include/Qt3DInput                                       \
	-I${Qt_DIR}/include/Qt3DLogic                                       \
	-I${Qt_DIR}/include/Qt3DQuick                                       \
	-I${Qt_DIR}/include/Qt3DQuickExtras                                 \
	-I${Qt_DIR}/include/Qt3DQuickInput                                  \
	-I${Qt_DIR}/include/Qt3DQuickRender                                 \
	-I${Qt_DIR}/include/Qt3DRender                                      \
	-I${Qt_DIR}/include/QtCore                                          \
	-I${Qt_DIR}/include/QtGamepad                                       \
	-I${Qt_DIR}/include/QtGui                                           \
	-I${Qt_DIR}/include/QtNetwork                                       \
	-I${Qt_DIR}/include/QtQml                                           \
	-I${Qt_DIR}/include/QtQuick                                         \
	-I${Qt_DIR}/include/QtWidgets                                       \
	-I${Qt_DIR}/include/QtXmlPatterns                                   \
	-i${BUILD_DIR}                      \
	${WORKSPACE}                        \
	2> ${WORKSPACE}/cppcheck.xml


# TODO Enable tests
# # Run tests. We or the result with true because we want this project to pass
# # regardless of whether tests pass or fail. The downstream job will parse the
# # test results, and pass or fail depending on the result of the tests.
# cd ${WORKSPACE}/build_dir
# # Clear out any old test results. Important for
# # continuous integration builds.
# rm -rf Testing
# ctest --no-compress-output -T Test || /bin/true

# vim: ts=2 sw=2 sts=0 noet ffs=unix :
