import Qt3D.Core 2.0
import Qt3D.Render 2.0
import QtQml 2.2

import Qt3D.Extras 2.0
import Qt3D.Input 2.0

import ".."

Entity {
	id: root

	property alias camera: cameraView.camera
	property rect viewport: null
	property Ship ship: null

	// onShipChanged: { console.debug("[UC] Changing UserCamera ship.  Ship=" + ship); }

	property SimpleCamera cameraView : SimpleCamera {
		id: cameraView

		ship: root.ship
		positionOffset: Qt.vector3d(-50,  30,   50).times(10)
		upVector:       Qt.vector3d(0, 0, 1)

		name: "user"
	}

	// OrbitCameraController { camera: cameraView.camera }

	// Not sure if useful, but there is some camera controlling code
	// https://youtu.be/29wCpA0DBZI?t=13m19s

}

// vim: ts=3 sw=3 sts=0 noet ffs=unix ft=qml :
