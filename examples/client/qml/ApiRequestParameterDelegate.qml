import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

ItemDelegate {
    id: delegate

    property string name
    property var value
    property int scope: -1

    function scopeName(scope) {
        switch (scope) {
        case 0:
            return qsTr("URL Path")
        case 1:
            return qsTr("URL Query")
        case 2:
            return qsTr("Header")
        default:
            return qsTr("Unknown")
        }
    }

    implicitWidth: delegateLayout.implicitWidth
    implicitHeight: delegateLayout.implicitHeight

    ColumnLayout {
        id: delegateLayout

        anchors.fill: parent

        RowLayout {
            Layout.fillWidth: true
            Layout.maximumHeight: 16

            Label {
                text: delegate.name
                font.pointSize: delegate.font.pointSize
                font.bold: true

                verticalAlignment: Text.AlignVCenter

                Layout.fillWidth: true
                Layout.fillHeight: true
            }

            RoundButton {
                text: "+"

                Layout.fillHeight: true
            }
        }

        TextArea {
            text: delegate.value
            wrapMode: Text.WrapAnywhere

            Layout.fillWidth: true
            Layout.fillHeight: true
        }

        ComboBox {
            id: scopeInput

            delegate: ItemDelegate {
                text: model.name
                width: scopeInput.width
                height: scopeInput.height
            }

            model: ListModel {
                ListElement {
                    name: qsTr("Url Path")
                }

                ListElement {
                    name: qsTr("Url Query")
                }

                ListElement {
                    name: qsTr("Header")
                }
            }

            currentIndex: delegate.scope

            Layout.fillWidth: true
        }
    }
}
