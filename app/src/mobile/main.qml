import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.3
import ReaderView 1.0
import Controller 1.0

ApplicationWindow {
    visible: true
    title: "Faster Than Sight"

    ColumnLayout {
        spacing: 0
        anchors.fill: parent

        CReaderView {
            anchors.fill: parent
            id: readerView

            // Initialization
            Component.onCompleted: {
                controller.setFontSize(controller.fontSizePoints())
                controller.openLastPosition()
            }

//            MouseArea {
//                anchors.fill: parent
//                onClicked: controller.togglePause()
//            }

            PinchArea {
                anchors.fill: parent

                property int textPointSize

                onPinchStarted: textPointSize = controller.fontSizePoints()
                onPinchUpdated: controller.setFontSize(textPointSize * pinch.scale)
            }

            Connections {
                target: controller
                onDisplayUpdateRequired: readerView.setText(text, showPivot, pivotCharacterIndex)
                onFontSizeChanged: readerView.setFontSizePoints(pointSize)
                onReaderStateChanged: btnPauseResume.text = state == 0 ? "Start" : "Pause"
            }
        }

        Button {
            width: 64
            height: 64
            id: btnPauseResume
            text: "Start"
            onClicked: controller.togglePause()
        }
    }
}
