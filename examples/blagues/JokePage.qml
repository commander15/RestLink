import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

import RestLink 1.0

Item {
    id: page

    property AppSettings settings

    ColumnLayout {
        anchors.fill: parent
        spacing: 5

        ColumnLayout {
            Layout.fillWidth: true
            Layout.preferredHeight: 300
            Layout.alignment: Qt.AlignHCenter | Qt.AlignTop
            Layout.margins: 12

            JokeView {
                id: view

                label: "Blague" + (jokeType === "dev" ? " de développeurs" : "")
                allowDev: page.settings.devEnabled

                jokeString: joke.jokeString
                jokeAnswer: joke.jokeAnswer
                jokeType: joke.jokeType

                Layout.fillWidth: true
                Layout.alignment: Qt.AlignTop

                Joke {
                    id: joke
                }
            }
        }

        Label {
            Layout.fillWidth: true
            text: "Made by <b>Amadou Benjamain</b><br>" + "Powered by <b>Blagues API</b>"
            horizontalAlignment: Text.AlignHCenter
            font.pointSize: 12
        }

        RoundButton {
            id: reloadButton

            text: qsTr("Une autre")
            font.pointSize: 14
            leftPadding: 18
            rightPadding: leftPadding

            Layout.minimumWidth: 128
            Layout.alignment: Qt.AlignHCenter
            Layout.bottomMargin: 12

            onClicked: function () {
                if (blinkTimer.running) {
                    blinkTimer.stop()
                    highlighted = false
                }

                joke.request.run()
            }

            Timer {
                id: blinkTimer

                property bool color: true

                interval: 500
                repeat: true
                running: true
                onTriggered: reloadButton.highlighted = (color = !color)
            }
        }
    }

    BusyIndicator {
        id: indicator
        running: joke.loading

        x: (parent.width / 2) - (width / 2)
        y: (parent.height / 2) - (height / 2)

        width: 128
        height: width
    }

    RoundButton {
        text: "Dev"
        highlighted: view.allowDev

        x: page.width - width - 32
        y: reloadButton.y

        onClicked: function () {
            view.allowDev = !view.allowDev
            page.settings.devEnabled = highlighted
        }
    }
}
