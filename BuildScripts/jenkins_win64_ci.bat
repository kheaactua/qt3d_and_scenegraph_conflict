REM This script performs the win64 CI build.

SET WORKSPACE=%1

REM define build and installation directories
SET BUILD_DIR=%WORKSPACE%\build_dir
SET INSTALL_DIR=%WORKSPACE%\install_dir

REM Execuatables
SET CXX="C:\Program Files (x86)\MSBuild\12.0\Bin\amd64\msbuild.exe"
SET CPPCHECK="C:\Program Files (x86)\Cppcheck\cppcheck.exe"

REM Note, here we do not delete the build_dir each time,
REM since we just want to build and integrate the new commits,
REM instead of a whole clean build each time.
mkdir %BUILD_DIR%
cd %BUILD_DIR%

SET Qt_VERSION_RELEASE=5
SET Qt_VERSION_MAJOR=7
SET Qt_VERSION_MINOR=1

SET BUILD_TYPE=Release
SET Qt_DIR=c:/Qt/Qt%Qt_VERSION_RELEASE%.%Qt_VERSION_MAJOR%.%Qt_VERSION_MINOR%/%Qt_VERSION_RELEASE%.%Qt_VERSION_MAJOR%/msvc2013_64
SET SOURCE_DIR=..

REM Run CMake
cmake                                        ^
	-G"Visual Studio 12 2013 Win64"            ^
	-DQt_VERSION_RELEASE=%Qt_VERSION_RELEASE%  ^
	-DQt_VERSION_MAJOR=%Qt_VERSION_MAJOR%      ^
	-DQt_VERSION_MINOR=%Qt_VERSION_MINOR%      ^
	-DQt_DIR=%Qt_DIR%                          ^
	-DCMAKE_BUILD_TYPE=%BUILD_TYPE%            ^
	%SOURCE_DIR%


CALL Set_Path_%BUILD_TYPE%.bat

REM Build the source
%CXX% /m:2 /property:Configuration=%BUILD_TYPE% ALL_BUILD.vcxproj
if ERRORLEVEL 1 goto :showerror


REM Run cppcheck (static code analysis) on the source.  cppcheck must be run
REM from the top level workspace directory so the paths will be correct:
REM
REM Include paths were taken from the linux file, and ultimately taken from
REM compile_commands.json.  In the future, we should attempt the --project flag
REM here.
cd %WORKSPACE%\..
%CPPCHECK% -j 4                                                 ^
	--enable=all                                                  ^
	--suppressions-list=%WORKSPACE%\BuildScripts\suppressions.txt ^
	--inconclusive                                                ^
	--xml --xml-version=2                                         ^
	-I%WORKSPACE%/app/include                                        ^
	-I%Qt_DIR%/include                                               ^
	-I%Qt_DIR%/include/Qt3DCore                                      ^
	-I%Qt_DIR%/include/Qt3DInput                                     ^
	-I%Qt_DIR%/include/Qt3DLogic                                     ^
	-I%Qt_DIR%/include/Qt3DQuick                                     ^
	-I%Qt_DIR%/include/Qt3DQuickExtras                               ^
	-I%Qt_DIR%/include/Qt3DQuickInput                                ^
	-I%Qt_DIR%/include/Qt3DQuickRender                               ^
	-I%Qt_DIR%/include/Qt3DRender                                    ^
	-I%Qt_DIR%/include/QtCore                                        ^
	-I%Qt_DIR%/include/QtGamepad                                     ^
	-I%Qt_DIR%/include/QtGui                                         ^
	-I%Qt_DIR%/include/QtNetwork                                     ^
	-I%Qt_DIR%/include/QtQml                                         ^
	-I%Qt_DIR%/include/QtQuick                                       ^
	-I%Qt_DIR%/include/QtWidgets                                     ^
	-I%Qt_DIR%/include/QtXmlPatterns                                 ^
	-i%BUILD_DIR%    ^
	%WORKSPACE% 2> %WORKSPACE%\cppcheck.xml



rem REM TODO Enable tests
rem REM Run tests. We or the result with verify because we want this project to
rem REM pass regardless of whether tests pass or fail. The downstream job will
rem REM parse the test results, and pass or fail depending on the
rem REM result of the tests.
rem cd %WORKSPACE%\build_dir
rem REM Clear out any old test results. Important for continuous integration
rem REM builds.
rem rmdir /Q /S Testing
rem ctest --no-compress-output -T Test -C Release || verify > NUL


REM Generate the installer package:
cd %BUILD_DIR%
%CXX% /m:2 /property:Configuration=%BUILD_TYPE% PACKAGE.vcxproj
if ERRORLEVEL 1 goto :showerror

goto :EOF

REM Handle build errors:
:showerror
echo Build error occurred
exit %ERRORLEVEL%

REM vim: ts=2 sw=2 sts=0 noet ffs=unix :
