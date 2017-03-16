import Qt3D.Core 2.0
import Qt3D.Render 2.0

import Qt3D.Extras 2.0
import Qt3D.Input 2.0

import ".."

Entity {
	id: root
	property var cameras: [user, top, back]

	property alias viewportUser: user.viewport
	property alias viewportTop:  top.viewport
	property alias viewportBack: back.viewport

	property Ship ship: null

	property UserCamera user : UserCamera {
		id: user
		ship: root.ship
		// onShipChanged: { console.debug("[CameraSet] Changing UserCamera ship.  Ship=" + ship); }
	}
	property TopCamera top   : TopCamera {
		id: top
		ship: root.ship
	}
	property BackCamera back : BackCamera {
		id: back
		ship: root.ship
	}
}

// vim: ts=3 sw=3 sts=0 noet ffs=unix ft=qml :
