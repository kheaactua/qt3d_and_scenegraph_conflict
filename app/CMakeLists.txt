project(app)
cmake_minimum_required(VERSION 3.0)

find_package(Threads)

list(APPEND Qt_COMPONENTS Gui)
list(APPEND Qt_COMPONENTS Widgets)
list(APPEND Qt_COMPONENTS Qml)
list(APPEND Qt_COMPONENTS Quick)
list(APPEND Qt_COMPONENTS 3DCore)
list(APPEND Qt_COMPONENTS 3DInput)
list(APPEND Qt_COMPONENTS 3DQuick)
list(APPEND Qt_COMPONENTS 3DRender)
list(APPEND Qt_COMPONENTS 3DQuickInput)
list(APPEND Qt_COMPONENTS 3DQuickRender)
list(APPEND Qt_COMPONENTS 3DQuickExtras)
list(APPEND Qt_COMPONENTS XmlPatterns)

foreach(pkg IN ITEMS ${Qt_COMPONENTS})
	# Qt_DEPS are our link targets.
	list(APPEND Qt_DEPS Qt${Qt_VERSION_RELEASE}::${pkg})
endforeach()
unset(pkg_name)
find_package(Qt${Qt_VERSION_RELEASE} REQUIRED COMPONENTS ${Qt_COMPONENTS})


set(MyExe app)

set(MODELS
	${CMAKE_SOURCE_DIR}/app/data/Ship_meteor_N010311.3DS
)

set(QGLTF_BIN ${Qt_DIR}/bin/qgltf)
if(WIN32)
	set(QGLTF_BIN "${QGLTF_BIN}.exe")
endif()
if (NOT EXISTS ${QGLTF_BIN})
	message(FATAL_ERROR "Cannot find Qt's qGLTF binary.  Looking for ${QGLTF_BIN}")
endif()
set(QGLTF_OUT ${CMAKE_CURRENT_SOURCE_DIR}/data/qgltf)
file(MAKE_DIRECTORY ${QGLTF_OUT})

set(SHIP_DATA ${QGLTF_OUT}/Ship_meteor_N010311.qgltf ${QGLTF_OUT}/Ship_meteor_N010311_1.bin ${QGLTF_OUT}/Ship_meteor_N010311_shader_1_f.frag ${QGLTF_OUT}/Ship_meteor_N010311_shader_1_f_core.frag ${QGLTF_OUT}/Ship_meteor_N010311_shader_1_v.vert ${QGLTF_OUT}/Ship_meteor_N010311_shader_1_v_core.vert ${QGLTF_OUT}/Ship_meteor_N010311.qrc)
set(DATA ${SHIP_DATA})
ADD_CUSTOM_COMMAND(
	OUTPUT ${SHIP_DATA}
	MAIN_DEPENDENCY ${MODELS}
	PRE_BUILD
	COMMAND "${Qt_DIR}/bin/qgltf" "-t" "-g" "-d" ${QGLTF_OUT} ${MODELS}
)


# # TODO make this only run if it has to
# execute_process(
# COMMAND
#		"/opt/Qt5.7.0/5.7/gcc_64/bin/qgltf"
#		"-t" "-g"
#		"-d" ${QGLTF_OUT}
#		${MODELS}
# )
file(GLOB QGLTF_FILES ${QGLTF_OUT}/*)

set(QRC)
file(GLOB_RECURSE QRC
	${CMAKE_CURRENT_SOURCE_DIR}/qml/*.qrc
)
list(APPEND QRC ${CMAKE_CURRENT_SOURCE_DIR}/qml.qrc)

foreach (qrcfile ${QRC})
	QT5_ADD_RESOURCES(RESOURCES_RCC ${qrcfile})
endforeach()
QT5_ADD_RESOURCES(RESOURCES_RCC ${QGLTF_OUT}/Ship_meteor_N010311.qrc)

file(GLOB_RECURSE QML
	${CMAKE_CURRENT_SOURCE_DIR}/qml/*.qml
)
file(GLOB_RECURSE SOURCE
	${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
)
file(GLOB Q_OBJS
	${CMAKE_CURRENT_SOURCE_DIR}/include/app/qobjs/*.h
)

# Manually perform automoc.  Calling this manually since Qt doesn't like when
# the moc objects are in a different directory.
qt5_wrap_cpp(app_qobjs_moc ${Q_OBJS} OPTIONS -I${Qt_DIR}/include/QtQml)

add_library(app_lib
	${SOURCE}
	${app_qobjs_moc}
)
target_include_directories(app_lib PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/include
)
target_link_libraries(app_lib
	${Qt_DEPS}
)
if(WIN32)
	# Lib required for adjusting screen brightness
	target_link_libraries(app_lib Dxva2)
endif()

set(DEBUG_POINTS 1 CACHE BOOL "Define the DEBUG_POINTS macro")
# Hard coding this as 1
target_compile_definitions(app_lib PUBLIC DEBUG_POINTS=1)
if (SIMULATE_GROUND_POINTS)
	target_compile_definitions(app_lib PUBLIC SIMULATE_GROUND_POINTS=1)
endif()
if (QSG_RENDERER_DEBUG)
	target_compile_definitions(app_lib PUBLIC QSG_RENDERER_DEBUG=${QSG_RENDERER_DEBUG})
endif()
if (QSG_VISUALIZE)
	target_compile_definitions(app_lib PUBLIC QSG_VISUALIZE=${QSG_VISUALIZE})
endif()


add_executable(${MyExe} main.cpp
	${RESOURCES_RCC}
	${QML}
	${QRC}
)
target_link_libraries(${MyExe} app_lib ${THREADS_LIB})



####################################################################
# Install rules for runtime distribution

INSTALL(
	TARGETS   ${MyExe}
	RUNTIME   DESTINATION ${INSTALL_BIN_REL_DIR}
	COMPONENT ${MY_COMPONENT_APP}
)

# Plugins required for the 3D Models
INSTALL(
	DIRECTORY   ${Qt_DIR}/plugins/sceneparsers ${Qt_DIR}/plugins/imageformats
	DESTINATION ${INSTALL_BIN_REL_DIR}
	COMPONENT   ${MY_COMPONENT_APP}
)

SET(INSTALLED_TARGET "${INSTALL_BIN_DIR}/${MyExe}${exe_ext}")
# Call fixup_bundle() on installed applications
INSTALL(
	CODE "include(BundleUtilities)
fixup_bundle(\"${INSTALLED_TARGET}\" \"\" \"${RUN_PATH};\")
if(WIN32)
	# Use Qt's windows deployment tool to copy over all the dependencies.
	set(WINDEPLOY_BIN ${Qt_DIR}/bin/windeployqt)
	execute_process(
		COMMAND \"\${WINDEPLOY_BIN}\" --release --qmldir ${CMAKE_CURRENT_SOURCE_DIR}/qml \"${MyExe}${exe_ext}\"
		WORKING_DIRECTORY ${INSTALL_BIN_DIR}
	)
endif()

"
	COMPONENT ${MY_COMPONENT_APP}
)

# vim: ts=3 sw=3 sts=0 noet :
