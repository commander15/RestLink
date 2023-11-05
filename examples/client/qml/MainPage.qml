import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

import RestLink 1.0

Page {
    id: page

    property Api api

    ColumnLayout {
        anchors.fill: parent

        TabBar {
            currentIndex: swipe.currentIndex

            Layout.fillWidth: true
            Layout.minimumHeight: 32

            TabButton {
                text: qsTr("Configuration")
                onClicked: swipe.currentIndex = 0
            }

            TabButton {
                text: qsTr("Request")
                onClicked: swipe.currentIndex = 1
            }
        }

        SwipeView {
            id: swipe

            clip: true

            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.margins: 9

            ApiConfigPage {
                api: page.api
            }

            ApiRequestPage {
                api: page.api
            }
        }
    }
}
