import QtQuick 2.7
import QtGraphicalEffects 1.0

/**
 * Viewport slidable partition, originally developed perform vertical and
 * horizontal slides, but has been reduced to simply horizontal (vertical
 * parittion that slides left to right)
 */
Rectangle {
	id: root

	color: "transparent"

	// Size of the sliding bar
	property int barWidth: 50
	property int horizontalPartitionWidth: 1

	// Alias to x percentage of parent, easier for understand for outside
	// objects which interface with this
	property real position: x/parent.width

	// Size of the tab grip
	property int gripHeight: 78

	height: parent.height
	width:  root.barWidth

	property string currentState: 'fill'

	// Positions (percentage of openess of left partition)
	property real positionSplit:  0.70
	property real positionFill:   0.99
	property real horizontalPartitionHeight: 0.50

	x: root.positionFill * parent.width

	Connections {
		target: window
		onWidthChanged: {
			x = root.positionFill * parent.width;
		}
	}

	onXChanged: {
		if ((x + width) > parent.width) x = (parent.width - width);
		if ((x-sliderGrip.width) < 0) x = sliderGrip.width;
	}

	states: [
		State {
			name: 'fill'; when: currentState == 'fill'
			PropertyChanges { target: root; x: parent.width*root.positionFill }
		}
		, State {
			name: 'split'; when: currentState == 'split'
			PropertyChanges { target: root; x: parent.width*root.positionSplit }
		}
	]

	transitions: [
		Transition {
			from: 'fill'; to: 'split'; reversible: true;
			NumberAnimation { properties: 'x'; duration: 300; easing.type: Easing.OutCubic }
		}
	]

	Rectangle {
		id: sliderBar
		height: root.height
		width:  root.barWidth
		color: theme.cControl

		Rectangle {
			id: sliderGrip
			color: theme.cControl
			radius: 2

			width: parent.width * 0.8
			height: gripHeight

			anchors {
				horizontalCenter: parent.horizontalCenter;
				verticalCenter:   parent.verticalCenter;
			}

			Image {
				id: gripImg
				height: gripHeight
				width:  gripHeight/5
				source: 'qrc:/icons/icon_sidebar_handle.svg'

				anchors {
					horizontalCenter: parent.horizontalCenter;
					top: parent.top;

					verticalCenter: parent.verticalCenter;
				}
			}

			ColorOverlay {
				anchors.fill: gripImg
				source: gripImg
				color: 'white'
			}


			MouseArea {
				id: dragTabArea
				anchors.fill: parent

				onPressed: { root.currentState = root.currentState == 'fill' ? 'split' : 'fill' }
			}
		}
	}

	Rectangle {
		id: horizontalPartition

		color: theme.cControl

		width:  root.parent.width - root.x + barWidth
		height: horizontalPartitionWidth

		x: root.parent.x + barWidth
		y: (horizontalPartitionHeight * parent.height) - (height/2)
	}
}

// vim: ts=3 sw=3 sts=0 noet ffs=unix ft=qml :
