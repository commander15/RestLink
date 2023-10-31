import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Rectangle {
    id: view
    radius: 6
    implicitHeight: typeLabel.implicitHeight + jokeLabel.implicitHeight + answerLabel.implicitHeight + (3 * 6) + 9

    property string label: "Blague"

    property string jokeString
    property string jokeAnswer
    property string jokeType

    ColumnLayout {
        anchors.fill: parent
        anchors.leftMargin: 6
        anchors.rightMargin: 6
        anchors.topMargin: 6
        anchors.bottomMargin: 6

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
