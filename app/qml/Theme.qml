import QtQuick 2.7

Item {
	property string cControl:             "#FF131C33"
	
	property string cControlActive:       "#343c4f"

	// property string cSceneClear:          "#0A0E1A" // this breaks everything? decimal: [0.33, 0.055, 0.10], "#0A0E1A"=[0.039, 0.055, 0.10], 0x55=0.33
	// This is effectively the background colour, and it requires 0 alpha,
	// otherwise points from the SceneGraph (e.g. ground) won't show
	property string cSceneClear:          Qt.rgba(parent.red, 0.055, 0.10, 1)

	property string cSettingsMenuOverLay: "#FF5FB8FF"

	property string cSettingsMenuFont:    "white"
	
	property string cPinnedObjectFont:	  "white"

	property string cVesselInfo:          "#CC283246"

	property string cTitleBar:            "#FF131C33"

	property string cPinnedObjectList:    "#CC131C33"
	
	property string cPinnedObject:		  "#21283E"
	
	property string cGrid:                "#283246"

	property string cPopUpInfo:           "#FA343c4f"
}

// vim: ts=3 sw=3 sts=0 noet ffs=unix ft=qml :
