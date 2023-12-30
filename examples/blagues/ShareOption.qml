import QtQuick 2.14
import QtQuick.Controls 2.14
import QtQuick.Layouts 1.14

Item {
    id: option

    RowLayout {
        anchors.fill: parent

        RoundButton {
            id: imageShareButton

            icon.source: "icons/share_image.png"
            icon.color: shareMenu.iconColor
            icon.width: shareLayout.buttonSize
            icon.height: shareLayout.buttonSize

            visible: shareButton.expanded

            Layout.fillHeight: true
        }

        RoundButton {
            id: textShareButton

            icon.source: "icons/share_text.png"
            icon.color: shareMenu.iconColor
            icon.width: shareLayout.buttonSize
            icon.height: shareLayout.buttonSize

            visible: shareButton.expanded

            Layout.fillHeight: true
        }

        RoundButton {
            id: shareButton

            property bool expanded: false

            icon.source: "icons/share.png"
            icon.color: shareMenu.iconColor
            icon.width: shareLayout.buttonSize
            icon.height: shareLayout.buttonSize

            onClicked: expanded = !expanded

            Layout.fillWidth: true
            Layout.fillHeight: true
        }
    }
}
