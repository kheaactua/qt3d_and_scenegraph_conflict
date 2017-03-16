import Qt3D.Core 2.0
import Qt3D.Render 2.0
import Qt3D.Extras 2.0

import ".."

Entity {
	id: root

	property Ship ship: null

	// Stores the offset from center.  Useful to save here and remove redundancy
	// below
	property vector3d positionOffset:   Qt.vector3d(-30,  30,  50)
	property vector3d viewCenterOffset: Qt.vector3d(0,     0,   0)
	property vector3d upVector:         Qt.vector3d(0,     0,   1)

	// Name, only used for debugging for now
	property string name: ""

	// Perspective Defaults..
	property real fieldOfView:  45;
	property real nearPlane:    0.01;
	property real farPlane:     100000.0

	property var projectionType: CameraLens.PerspectiveProjection

	// Orthogonal defaults
	// http://doc.qt.io/qt-5/qmatrix4x4.html#viewport
	property alias top:    camera.top
	property alias right:  camera.right
	property alias left:   camera.left
	property alias bottom: camera.bottom

	// // Debugging property
	// property int dcount: 0

	property Camera camera : Camera {
		id: camera
		projectionType:  root.projectionType
		fieldOfView:     root.fieldOfView
		nearPlane:       root.nearPlane
		farPlane:        root.farPlane

		// Using a functional aspect ratio to prevent scaling when the viewports
		// are resized.  Orthogonal cameras don't use aspect ratio though, so
		// this won't work on the Top and Back cameras
		aspectRatio: (window.width * viewport.width) / (window.height * viewport.height)

		// Recall, currently the actual ship doesn't move, its `position` is just
		// an offset of the world.  Thus, we don't focus on
		// `ship.position.plus(root.positionOffset.plus(ship.offset))`, but just
		// on the pre-defined offset-from-ship-center.
		// This will be changed soon with the introducion of the origo.
		position:   root.positionOffset.plus(ship.offset)
		viewCenter: ship.offset

		upVector:   root.upVector

		onPositionChanged: {
			// // Debugging code
			// if (root.name == "top" && dcount%60 === 0) {
			// 	var props = ""
			// 	if (CameraLens.PerspectiveProjection === camera.projectionType) {
			// 		props = "F=(" + top + ", " + right + ", " + bottom + ", " + left + ")";
			// 	} else {
			// 		props = "F=(" + nearPlane + ", " + farPlane + ")";
			// 	}
			// 	console.log("[SC:" + root.name +"] s.P=" + ship.position + ", c.P = " + position + ", D_p=" + ship.position.minus(position) + ", c.vc=" + viewCenter + ", D_svc=" + ship.position.minus(viewCenter))
			// }
			// dcount = (dcount + 1) % 60;
		}
	}


	// Simply having this transformation makes aiming the cameras super
	// annoying, and will effect their initial states.  I think the root cause
	// is the camera is initially looking elsewhere
	// Transform {
	// 	id: transform
	// 	matrix: {
	// 		// Loads an indentity matrix.  Even returning just this will still
	// 		// allow the camera to rotate
	// 		var m = Qt.matrix4x4(); // http://doc.qt.io/qt-5/qmatrix4x4.html
   //
	// 		m.lookAt(camera.position, camera.viewCenter, camera.upVector);
	// 		return m;
	// 	}
	// }

	components: [ camera ]
}

// vim: ts=3 sw=3 sts=0 noet ffs=unix ft=qml :
