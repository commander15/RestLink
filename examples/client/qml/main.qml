import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

import RestLink 1.0

ApplicationWindow {
    id: win

    title: "RestLink"
    width: 400
    height: 800
    visible: true

    header: AppToolBar {}

    background: Rectangle {
        color: "dark gray"

        Image {
            anchors.fill: parent
            source: "../icons/logo.png"
            fillMode: Image.PreserveAspectFit
        }
    }

    StackView {
        id: stack

        initialItem: MainPage {
            api: restApi
        }

        anchors.fill: parent
    }

    Api {
        id: restApi
    }
}
