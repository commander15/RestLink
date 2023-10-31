import QtQuick 2.14
import QtQuick.Controls 2.14
import QtQuick.Window 2.14
import QtQuick.Layouts 1.14

Item {
    id: win
    width: 300
    height: 280
    visible: true

    ListView {
        id: view

        delegate: MediaDelegate {
            poster: "file:///home/commander/Pictures/IMG-20220513-WA0040.jpg"
            title: "Marvel Rising in a Multiversal World"
            overview: "Taken in a universe of madness, the hero, sadly begin to die"
            popularity: 10 - model.index
            year: 2022
            width: view.width
        }

        model: 11

        spacing: 5

        anchors.fill: parent
        anchors.margins: 9
    }
}
