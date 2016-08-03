import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.3
import QtQuick.Controls.Material 2.0
import ReaderView 1.0
import Controller 1.0

ApplicationWindow {
    Material.theme: Material.Dark
    Material.accent: Material.Orange

    id: window
    visible: true
    title: "Faster Than Sight"

    CReaderView {
        anchors.fill: parent
        id: readerView

        // Initialization
        Component.onCompleted: {
            controller.setFontSize(controller.fontSizePoints())
            readerView.setText(qsTr("Loading..."), false, -1);
            if (!controller.openLastPosition()) {
                readerView.setText(qsTr("No text loaded."), false, -1);
            }
        }

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
            onReaderStateChangedInt: btnPauseResume.text = (state != 0 ? "Start" : "Pause")
        }
    }

    RowLayout {
        anchors.bottom: readerView.bottom
        anchors.horizontalCenter: readerView.horizontalCenter
        anchors.bottomMargin: 32

        Button {
            text: "Prev. chapter"
            onClicked: controller.toPreviousChapter()
        }

        Button {
            text: "Prev. paragraph"
            onClicked: controller.toPreviousParagraph()
        }

        Button {
            id: btnPauseResume
            text: "Start"
            onClicked: controller.togglePause()
        }

        Button {
            text: "Next paragraph"
            onClicked: controller.toNextParagraph()
        }

        Button {
            text: "Next chapter"
            onClicked: controller.toNextChapter()
        }
    }
}
