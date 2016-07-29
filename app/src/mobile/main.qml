import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.3
import ReaderView 1.0

ApplicationWindow {
    visible: true
    title: "Faster Than Sight"

    ColumnLayout {
        spacing: 0
        anchors.fill: parent

        CReaderView {
            Layout.fillHeight: true
            Layout.fillWidth: true

            Layout.minimumHeight: 0
            Layout.maximumHeight: Number.POSITIVE_INFINITY

            Layout.minimumWidth: 0
            Layout.maximumWidth: Number.POSITIVE_INFINITY

            objectName: "_readerView"
        }
    }
}
