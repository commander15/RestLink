import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

ApplicationWindow {
    id: win

    title: "Blagues"
    width: 400
    height: 800
    visible: true
    header: MyToolBar {}
    background: Rectangle {
        color: "dark gray"

        Image {
            anchors.fill: parent
            source: "icons/logo.png"
            fillMode: Image.PreserveAspectFit
        }
    }

    StackView {
        id: stack
        anchors.fill: parent
        initialItem: jokePage
    }

    Component {
        id: jokePage

        JokePage {
            settings: appSettings
        }
    }

    AppSettings {
        id: appSettings
    }
}
