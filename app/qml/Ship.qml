import QtQuick 2.7
import Qt3D.Core 2.0
import Qt3D.Render 2.0
import Qt3D.Extras 2.0

Entity {
	id: root

// TODO look up singleton

	// This is the position of the ship relative to the start of the simulation.
	// The actual ship stays at the origin and everything moves around it.
	// Everything basically watches this property for movement.
	property real tx: 0.0
	property real ty: 0.0
	property real tz: -2.0
	property vector3d position: Qt.vector3d(tx, ty, tz)

	// Direction of the ship
	property real yawAngle: 0.0

	// The offset from the ship model to whatever we want to consider the
	// center of the ship.  This is what the cameras point to, for example
	property vector3d offset: Qt.vector3d(0, 0, -2)

	// Scale of the ship model
	property real scaleFactor: 1/800
	property vector3d scale: Qt.vector3d(root.scaleFactor, root.scaleFactor, root.scaleFactor)

	property real turnDuration: 5000

	property Transform transform: Transform {
		id: transform
		scale3D: root.scale

		property real pitchAngle: 0.0

		rotation: fromAxisAndAngle(Qt.vector3d(0, 1, 0), pitchAngle)
	}

	property SceneLoader shipObj: SceneLoader {
		id: shipObj
		source: "qrc:/models/Ship_meteor_N010311.qgltf"
		onStatusChanged: {
			console.log('Loaded Ship, status: ' + (status == SceneLoader.Ready ? "ready" : "not ready"));
		}
	}

	components: [transform, shipObj]


}

// vim: ts=3 sw=3 sts=0 noet ffs=unix ft=qml :
