import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 2.15

import QtCore

import RestLink 2.0

ApplicationWindow {
    id: window

    width: 400
    height: 800
    visible:  true

    header: AppBar {
        id: appBar
        title: "Store"
    }

    footer: TabBar {
        currentIndex: swipe.currentIndex
        onCurrentIndexChanged: swipe.currentIndex = currentIndex

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
    }

    SwipeView {
        id: swipe

        anchors.fill: parent

        ProductListPage {
            api: store
        }

        CategoryListPage {
            api: store
        }

        ConfigurationPage {
            api: store
        }
    }

    StoreApi {
        id: store
    }

    Settings {
        //property alias firstRun: store.firstRun
    }
}
