import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Rectangle {
    id: view

    property string label: "Blague" + (jokeType == "dev" ? " de developpeur" : "")
    readonly property alias labelItem: typeLabel

    property int  jokeId: 0
    property string jokeString
    property string jokeAnswer
    property string jokeType

    radius: 6
    implicitHeight: layout.implicitHeight + (3 * 6) + 9

    ColumnLayout {
        id: layout

        readonly property int margins: 6

        anchors.fill: parent
        anchors.leftMargin: margins
        anchors.rightMargin: margins
        anchors.topMargin: margins
        anchors.bottomMargin: margins

        Label {
            id: typeLabel
            text: view.label
            font.pointSize: 16
            color: "gray"
            wrapMode: Text.WordWrap

            Layout.fillWidth: true
        }

        Label {
            id: jokeLabel
            text: view.jokeString
            font.pointSize: 14
            font.bold: true
            wrapMode: Text.WordWrap

            Layout.fillWidth: true
        }

        Label {
            id: answerLabel
            font.pointSize: 13
            text: view.jokeAnswer
            wrapMode: Text.WordWrap

            Layout.fillWidth: true
            Layout.topMargin: 6
        }
    }
}
