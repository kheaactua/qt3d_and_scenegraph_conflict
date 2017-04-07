import Qt3D.Core 2.0
import Qt3D.Render 2.0

import "Cameras"

RenderSettings {
	id: root

	property CameraSet cameraSet: null

	property real userViewWidth: 0.79
	property real topOrthoViewHeight: 0.50

	property alias viewportUserRect: userViewport.normalizedRect
	property alias viewportBackRect: backViewport.normalizedRect

	// renderPolicy: RenderSettings.Always

	activeFrameGraph: Viewport {
		id: viewport
		normalizedRect: Qt.rect(0.0, 0.0, 1.0, 1.0)

		RenderSurfaceSelector {
			ClearBuffers {
				buffers:    ClearBuffers.ColorDepthBuffer
				clearColor: Qt.rgba(parent.red, 0.055, 0.10, 0)

				NoDraw {}
			}

			// I think this can save GPU time by only rendering what can be seen?
			FrustumCulling {}

			Viewport {
				id: userViewport
				normalizedRect: shipScene.viewportUserRect

				CameraSelector {
					id: userCameraSelectorViewport
					camera: cameraSet.user.camera
				}
			}

			Viewport {
				id: topViewport
				normalizedRect: shipScene.viewportTopRect
				CameraSelector {
					id: topCameraSelectorViewport
					camera: cameraSet.top.camera
				}
			}

			Viewport {
				id: backViewport
				normalizedRect: shipScene.viewportBackRect
				CameraSelector {
					id: backCameraSelectorViewport
					camera: cameraSet.back.camera
				}
			}
		}
	}

}

// vim: ts=3 sw=3 sts=0 noet ffs=unix ft=qml :
