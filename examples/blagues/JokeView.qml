import QtQuick 2.14
import QtQuick.Controls 2.14
import QtQuick.Layouts 1.14

BasicJokeView {
    id: view

    //jokeString: "C'est quoi le truc qu'on appelle Windows partout svp c urgent ?"
    //jokeAnswer: "Juste le pire système au monde"
    //jokeType: "dev"

    Rectangle {
        id: shareMenu

        property color iconColor: "red"

        radius: 16

        x: view.width - (width + 6)
        y: view.labelItem.y + 6

        height: view.labelItem.height

        visible: false

        implicitWidth: shareLayout.implicitWidth
        implicitHeight: shareLayout.implicitHeight

        RowLayout {
            id: shareLayout

            readonly property int buttonSize: shareMenu.radius

            anchors.fill: parent

            Rectangle {
                color: "gray"

                visible: shareButton.expanded

                implicitWidth: shareButtonLayout.implicitWidth
                implicitHeight: shareButtonLayout.implicitHeight

                Layout.fillHeight: true

                RowLayout {
                    id: shareButtonLayout

                    anchors.fill: parent

                    RoundButton {
                        id: imageShareButton

                        icon.source: "icons/share_image.png"
                        icon.color: shareMenu.iconColor
                        icon.width: shareLayout.buttonSize
                        icon.height: shareLayout.buttonSize

                        Layout.preferredWidth: height
                        Layout.fillHeight: true
                    }

                    RoundButton {
                        id: textShareButton

                        icon.source: "icons/share_text.png"
                        icon.color: shareMenu.iconColor
                        icon.width: shareLayout.buttonSize
                        icon.height: shareLayout.buttonSize

                        Layout.preferredWidth: height
                        Layout.fillHeight: true
                    }
                }
            }

            RoundButton {
                id: shareButton

                property bool expanded: false

                icon.source: "icons/share.png"
                icon.color: "green"
                icon.width: shareLayout.buttonSize
                icon.height: shareLayout.buttonSize

                onClicked: expanded = !expanded

                Layout.preferredWidth: height
                Layout.fillHeight: true
            }
        }
    }
}
