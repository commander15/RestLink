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

        ListView {
            id: view

            delegate: JokeView {
                label: "Blague" + (jokeType === "dev" ? " de développeurs" : "")

                jokeString: model.jokeString
                jokeAnswer: model.jokeAnswer
                jokeType: model.jokeType

                width: view.width - (view.leftMargin + view.rightMargin)
            }

            leftMargin: 6
            rightMargin: 6
            topMargin: 6
            bottomMargin: 6
            spacing: 9

            model: ListModel {
                id: jokeModel

                onCountChanged: view.positionViewAtEnd()

                Component.onCompleted: function() {
                    var jokes = JSON.parse(page.settings.jokeModel);
                    for (var i = 0; i < jokes.length; ++i)
                        append(jokes[i]);
                }

                Component.onDestruction: function() {
                    var jokes = Array();
                    for (var i = 0; i < count; ++i)
                        jokes.push(get(i));
                    page.settings.jokeModel = JSON.stringify(jokes);
                }
            }

            clip: true

            Layout.fillWidth: true
            Layout.fillHeight: true

            Layout.bottomMargin: 6

            Joke {
                id: joke

                allowDev: page.settings.devEnabled

                onJokeChanged: function() {
                    var object = Object();
                    object.jokeString = jokeString;
                    object.jokeAnswer = jokeAnswer;
                    object.jokeType = jokeType;
                    jokeModel.append(object);

                    if (jokeModel.count > 30)
                        jokeModel.remove(0);
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
                running: page.settings.firstRun
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
        highlighted: joke.allowDev

        x: page.width - width - 32
        y: reloadButton.y

        onClicked: function () {
            page.settings.devEnabled = !page.settings.devEnabled
        }
    }
}
