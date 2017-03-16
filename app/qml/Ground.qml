import QtQuick 2.7
import Qt3D.Core 2.0

import matt 1.0 as MM

Item {
	id: root

	property alias ship: sceneGraph.ship
	property alias userViewport: groundUserViewport.normalizedPaddedRect
	property alias topViewport:  groundTopViewport.normalizedPaddedRect
	property alias backViewport: groundBackViewport.normalizedPaddedRect

	function callSimulatePoints() {
		sceneGraph.callSimulatePoints();
	}

	MM.Ground {
		id: sceneGraph
		objectName: "ground"
		maxBufferPointCapacity: 1024*100*3
		pointSize: 1.7
		computeShader:  "qrc:/shaders/pointcloud.comp"
		vertexShader:   "qrc:/shaders/pointcloud.vert"
		fragmentShader: "qrc:/shaders/pointcloud.frag"


		// Debugging flag to deactivate the OpenGL
		deactivePoints: settings.value('deactivate_points')

		// Ship is provided after the scene is constructed.

		MM.Viewport {
			id: groundUserViewport
			normalizedRect: Qt.rect(0.0, 0.0, slider.position, 1.0)
			// Can't seem to use a camera selector.
			camera: shipScene.cameraSet.user.camera
			name: "user"
		}
		MM.Viewport {
			id: groundTopViewport
			normalizedRect: Qt.rect(slider.position, 0, 1-slider.position, 0.5)
			camera: shipScene.cameraSet.top.camera
			paddingLeft: slider.width
			name: "top"
		}
		MM.Viewport {
			id: groundBackViewport
			normalizedRect: Qt.rect(slider.position, 0.5, 1-slider.position, 0.5)
			camera: shipScene.cameraSet.back.camera
			paddingLeft: slider.width
			name: "bottom"
		}
	}
}

// vim: ts=3 sw=3 sts=0 noet ffs=unix ft=qml :
