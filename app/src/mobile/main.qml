import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.3
import ReaderView 1.0
import Controller 1.0

ApplicationWindow {
    visible: true
    title: "Faster Than Sight"

    // Initialization
    Component.onCompleted: controller.openLastPosition()

    MouseArea {
        anchors.fill: parent

        onClicked: {
            console.log("onClicked!")
            controller.togglePause()
        }
    }

    ColumnLayout {
        spacing: 0
        anchors.fill: parent

        CReaderView {
            anchors.fill: parent

            objectName: "_readerView"

            controller.onOnDisplayUpdateRequired: setText()
        }
    }
}
