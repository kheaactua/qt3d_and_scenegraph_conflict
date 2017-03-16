# Read up on %~dp0 at http://stackoverflow.com/questions/112055/what-does-d0-mean-in-a-windows-batch-file

macro(GENERATE_VISUAL_STUDIO_LAUNCH_SCRIPTS SOLUTION_NAME)

	file(WRITE 
		${CMAKE_CURRENT_BINARY_DIR}/Soln_${SOLUTION_NAME}_${CMAKE_BUILD_TYPE}.bat
 
"
echo Loading ${SOLUTION_NAME} environment...

@echo off
call ${CMAKE_BINARY_DIR}\\Set_Path_${CMAKE_BUILD_TYPE}.bat
@echo on

echo Launching ${SOLUTION_NAME}
@echo off
START %~dp0\\${SOLUTION_NAME}.sln
"
	)

endmacro(GENERATE_VISUAL_STUDIO_LAUNCH_SCRIPTS)
	
# vim: ts=4 : sw=4 : sts=0 : noet : ft=cmake : ffs=unix,dos :