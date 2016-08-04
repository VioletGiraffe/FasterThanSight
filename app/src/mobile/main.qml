import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.3
import QtQuick.Controls.Material 2.0
import ReaderView 1.0
import Controller 1.0

ApplicationWindow {
    Material.theme: Material.Dark
    Material.accent: "#FFFFC000"
    Material.primary: "#80000000"

    id: window
    visible: true
    title: "Faster Than Sight"

    header: ToolBar {
        id: toolBar

        Slider {
            anchors.fill: parent

            id: wpmSlider

            from: 100
            to: 800
            stepSize: 5
            value: controller.readingSpeed()

            onValueChanged: controller.setReadingSpeed(value)
        }
    }

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
            id: pinchArea

            property int textPointSize

            onPinchStarted: textPointSize = controller.fontSizePoints()
            onPinchUpdated: controller.setFontSize(textPointSize * pinch.scale)
        }

        MouseArea {
            anchors.fill: parent
            id: tapArea

            onClicked: controller.pauseReading()
        }

        Connections {
            target: controller
            onDisplayUpdateRequired: readerView.setText(text, showPivot, pivotCharacterIndex)
            onFontSizeChanged: readerView.setFontSizePoints(pointSize)
            onReaderStateChangedInt: {
                var reading = state == 0

                pinchArea.enabled = !reading;
                tapArea.enabled = reading
                buttons.visible = !reading

                toolBar.visible = !reading
                statusBar.visible = !reading

                if (reading) {
                    btnPauseResume.text = "Pause"
                }
                else {
                    btnPauseResume.text = "Start"
                }
            }
        }
    }

    RowLayout {
        anchors.bottom: readerView.bottom
        anchors.horizontalCenter: readerView.horizontalCenter
        anchors.bottomMargin: 32

        id: buttons

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

    footer: ToolBar {
        id: statusBar
    }
}
