import QtQuick 2.7
import Qt3D.Core 2.0
import Qt3D.Render 2.0
import Qt3D.Input 2.0
import Qt3D.Extras 2.0
import QtQuick.Controls 1.3

import "Cameras"

Entity {
	id: shipScene
	components: [viewport, inputSettings]

	// The viewports are set in the ground, and forwarded here.  This way we
	// ensure that the OpenGL and QML Renderer use the same viewport settings.
	property rect viewportUserRect: null
	property rect viewportTopRect:  null
	property rect viewportBackRect: null

	property CameraSet cameraSet: CameraSet {
		id: cameraSet
		ship: ship
		viewportUser: viewportUserRect
		viewportTop:  viewportTopRect
		viewportBack: viewportBackRect
	}

	property var renderer : ViewportFrameGraphs {
		id: viewport
		cameraSet: shipScene.cameraSet
	}

	property alias userViewWidth: viewport.userViewWidth
	property alias topOrthoViewHeight: viewport.topOrthoViewHeight

	InputSettings { id: inputSettings }

	property Entity sceneRoot: Entity {
		id: sceneRoot

		property Ship ship: Ship {
			id: ship
		}

	}
}

// vim: ts=3 sw=3 sts=0 noet ffs=unix ft=qml :
