import Qt3D.Core 2.0
import Qt3D.Render 2.0

import Qt3D.Extras 2.0
import Qt3D.Input 2.0

import ".."

Entity {
	id: root

	property alias camera: cameraView.camera
	property rect viewport
	property Ship ship: null

	// Frustrum scale
	property real fscale: 40

	property SimpleCamera cameraView : SimpleCamera {
		id: cameraView

		projectionType: CameraLens.OrthographicProjection

		ship: root.ship
		positionOffset: Qt.vector3d(0, 0, 25)
		upVector:       Qt.vector3d(1, 0, 0)

		top:    root.fscale / (camera.aspectRatio/2)
		right:  root.fscale
		bottom: -1 * root.fscale / (camera.aspectRatio/2)
		left:   -1 * root.fscale

		name: "top"
	}
}

// vim: ts=3 sw=3 sts=0 noet ffs=unix ft=qml :
