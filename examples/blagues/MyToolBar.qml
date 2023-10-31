import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

ToolBar {
    id: toolBar
    height: 64

    RowLayout {
        anchors.fill: parent
        spacing: 5

        Image {
            source: "icons/logo.png"
            fillMode: Image.PreserveAspectFit

            Layout.maximumWidth: 64
            Layout.fillHeight: true
            Layout.margins: 6
        }

        Label {
            Layout.fillWidth: true
            Layout.fillHeight: true
            font.pointSize: 24
            font.bold: true
            text: "Blagues"
            horizontalAlignment: Text.AlignLeft
            verticalAlignment: Text.AlignVCenter
        }
    }
}
