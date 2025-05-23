import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 2.15

MouseArea {
    id: delegate

    property color foregroundColor: "black"
    property color backgroundColor: "white"

    property var product

    implicitWidth: mainLayout.implicitWidth
    implicitHeight: 64

    Rectangle {
        radius: 6
        color: delegate.backgroundColor

        anchors.fill: parent

        RowLayout {
            id: mainLayout

            anchors.fill: parent
            anchors.margins: 6

            Image {
                Layout.fillHeight: true
            }

            ColumnLayout {
                Layout.fillWidth: true
                Layout.fillHeight: true

                RowLayout {
                    Layout.fillWidth: true

                    Label {
                        text: delegate.product.name
                        color: delegate.foregroundColor

                        font.pointSize: 14
                        font.bold: true

                        verticalAlignment: Label.AlignVCenter

                        Layout.fillHeight: true
                    }

                    Rectangle {
                        color: "red"
                        radius: 6

                        Layout.fillHeight: true
                        Layout.preferredWidth: category.implicitWidth + category.implicitWidth * 0.2

                        Label {
                            id: category

                            text: delegate.product.category.name
                            color: delegate.backgroundColor

                            font.pointSize: 14
                            font.bold: true

                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter

                            anchors.fill: parent
                        }
                    }
                }

                Label {
                    text: delegate.product.description
                    color: delegate.foregroundColor

                    Layout.fillWidth: true
                    Layout.fillHeight: true
                }
            }

            Label {
                text: delegate.product.price + "$ / " + delegate.product.stock.quantity
                color: delegate.foregroundColor

                font.pointSize: 18

                verticalAlignment: Label.AlignVCenter

                Layout.fillHeight: true
            }
        }
    }
}
