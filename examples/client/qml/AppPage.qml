import QtQuick 2.14
import QtQuick.Controls 2.14

Page {
    id: page

    default property alias data: view.data

    ScrollView {
        id: view

        anchors.fill: parent
    }
}
