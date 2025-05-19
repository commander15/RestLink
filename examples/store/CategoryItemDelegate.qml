import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 2.15

MouseArea {
    id: delegate

    property color foregroundColor: "black"
    property color backgroundColor: "white"

    property var product

    implicitHeight: 64

    Rectangle {
        radius: 6
        color: delegate.backgroundColor

        anchors.fill: parent

        RowLayout {
            anchors.fill: parent
            anchors.margins: 6

            Image {
                Layout.fillHeight: true
            }

            ColumnLayout {
                Layout.fillWidth: true
                Layout.fillHeight: true

                Label {
                    text: delegate.product.name
                    color: delegate.foregroundColor

                    font.pointSize: 14
                    font.bold: true

                    verticalAlignment: Label.AlignVCenter

                    Layout.fillWidth: true
                    Layout.fillHeight: true
                }

                Label {
                    text: delegate.product.description
                    color: delegate.foregroundColor

                    Layout.fillWidth: true
                    Layout.fillHeight: true
                }
            }
        }
    }
}
