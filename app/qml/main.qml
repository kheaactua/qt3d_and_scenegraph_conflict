import QtQuick 2.7
import Qt3D.Core 2.0
import Qt3D.Render 2.0
import Qt3D.Input 2.0
import Qt3D.Extras 2.0
import QtQuick.Controls 1.3
import QtQuick.Scene3D 2.0
import QtGraphicalEffects 1.0

import QtQuick.Layouts 1.3

import "Cameras"

Item {
	id: window
	visible: true
	anchors.fill: parent

	Rectangle {
		id: scene
		anchors.fill: parent
		color: "transparent"

		Scene3D {
			id: scene3d
			anchors.fill: scene
			aspects: ["input", "logic"]
			cameraAspectRatioMode: Scene3D.AutomaticAspectRatio
			focus: true

			ShipScene {
				id: shipScene
				objectName: "shipScene"
				userViewWidth: slider.position
				viewportUserRect: ground.userViewport
				viewportTopRect:  ground.topViewport
				viewportBackRect: ground.backViewport
			}
		} // scene3d

		Ground {
			// Put ground config options in the Ground object file itself.
			// Note, because this is run on afterRendering, items under it won't
			// be drawn ontop of it.
			id: ground
			ship: shipScene.sceneRoot.ship
		}

		ViewportSlider {
			id: slider
			height: scene.height
		}

	} // scene

	Rectangle {
		id: controlPanel
		anchors {
			left:  window.left
			bottom: window.bottom
			bottomMargin: 20
		}
		height: 100 - anchors.bottomMargin
		width: 100
		color: "transparent"

		ColumnLayout {

			anchors.fill: parent
			Layout.fillHeight: true
			Layout.fillWidth: true

			Button {
				text: "Simulate Ground Plane"
				onClicked: {
					console.debug("Simulating ground plane");
					ground.callSimulatePoints();
				}
			}
			Button {
				text: "Move ship"
				onClicked: {
					shipScene.sceneRoot.ship.tx += 20;
				}
			}
		}
	}
} // item

// vim: ts=3 sw=3 sts=0 noet ffs=unix ft=qml :
