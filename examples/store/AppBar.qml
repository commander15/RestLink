import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 2.15

import QtCore

import RestLink 2.0

ToolBar {
    id: appBar

    property alias title: titleLabel.text

    height: 64
    
    Label {
        id: titleLabel

        text: "Store"
        
        font.pointSize: 25
        font.bold: true
        
        verticalAlignment: Text.AlignVCenter
        
        anchors.fill: parent
        anchors.leftMargin: 12
    }
}
