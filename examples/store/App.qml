import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 2.15

import QtCore

import RestLink 2.0

ApplicationWindow {
    id: window

    width: 400
    height: 800
    visible: true

    header: AppBar {
        id: appBar
        title: "Store"
    }

    footer: TabBar {
        id: tabBar

        height: 0

        TabButton {
            text: "Products"
            icon.source: "file:///home/commander/Downloads/ChatGPT Image May 10, 2025, 02_34_14 PM.png"
            icon.color: "transparent"
            display: TabButton.TextUnderIcon
            height: 64
        }

        TabButton {
            text: "Categories"
            icon.source: "file:///home/commander/Downloads/ChatGPT Image May 10, 2025, 02_34_14 PM.png"
            icon.color: "transparent"
            display: TabButton.TextUnderIcon
            height: 64
        }

        PropertyAnimation on height {
            id: showTabBarAnimation
            to: 64
            duration: 300
            running: false
        }

        PropertyAnimation on height {
            id: hideTabBarAnimation
            to: 0
            duration: 300
            running: false
        }
    }

    Component {
        id: ui

        SwipeView {
            id: swipe

            Component.onCompleted: function () {
                tabBar.currentIndex = currentIndex
            }

            ProductListPage {
                api: store
                onLoaded: showTabBarAnimation.start()
                onTabHideRequested: hideTabBarAnimation.start()
            }

            CategoryListPage {
                api: store
            }

            ConfigurationPage {
                api: store
            }

            Connections {
                target: tabBar

                function onCurrentIndexChanged() {
                    swipe.currentIndex = tabBar.currentIndex
                }
            }
        }
    }

    StackView {
        id: stack

        BusyIndicator {
            running: parent.depth === 0
            width: 100
            height: 100
            anchors.centerIn: parent
        }

        anchors.fill: parent
    }

    StoreApi {
        id: store
        onInitialized: stack.push(ui)
    }

    Settings {
        //property alias firstRun: store.firstRun
    }
}
