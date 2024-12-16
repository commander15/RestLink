import QtQuick 2.14
import QtQuick.Controls 2.14
import QtQuick.Layouts 1.14

ItemDelegate {
    id: delegate

    property string poster
    property string title
    property string overview
    property real popularity: 0.0
    property int year: 2023

    property url imageUrl: "http://image.tmdb.org/t/p/w500"

    property color foregroundColor: "white"
    property color backgroundColor: "black"
    property color accentColor: "gray"

    width: 300
    height: 50

    implicitWidth: layout.implicitWidth
    implicitHeight: layout.implicitHeight

    Rectangle {
        color: delegate.backgroundColor
        radius: 6

        anchors.fill: parent

        RowLayout {
            id: layout

            spacing: 0
            anchors.fill: parent

            Label {
                text: delegate.year
                font.bold: true
                color: delegate.foregroundColor
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                rotation: -90

                Layout.leftMargin: implicitHeight / 2
                Layout.maximumWidth: implicitHeight
            }

            Image {
                source: (delegate.poster.length > 0 ? delegate.imageUrl + delegate.poster : "")
                sourceSize: Qt.size(64, 64)
                fillMode: Image.PreserveAspectFit

                Layout.preferredWidth: sourceSize.width
                Layout.fillHeight: true
            }

            ColumnLayout {
                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.margins: 5
                Layout.leftMargin: 0

                Label {
                    text: delegate.title
                    font.bold: true
                    color: delegate.foregroundColor

                    Layout.fillWidth: true
                    Layout.fillHeight: true

                    function reducedTitle() {
                        var t;
                        var idealLength = delegate.availableWidth / font.pixelSize * 1.4;
                        if (delegate.title.length > idealLength)
                            o = delegate.overview.substring(0, idealLength) + "...";
                        else
                            o = delegate.overview;
                        return o;
                    }
                }

                Label {
                    text: reducedOverview()
                    color: delegate.foregroundColor

                    Layout.fillWidth: true
                    Layout.fillHeight: true

                    function reducedOverview() {
                        var o;
                        var idealLength = delegate.availableWidth / font.pixelSize * 1.4;
                        if (delegate.overview.length > idealLength)
                            o = delegate.overview.substring(0, idealLength) + "...";
                        else
                            o = delegate.overview;
                        return o;
                    }
                }

                ProgressBar {
                    id: progress

                    value: delegate.popularity
                    from: 0
                    to: 10

                    contentItem: Item {
                        implicitWidth: 50
                        implicitHeight: 4

                        Rectangle {
                            width: progress.visualPosition * parent.width
                            height: parent.height
                            radius: 3
                            color: delegate.accentColor
                        }
                    }

                    background: Rectangle {
                        color: delegate.foregroundColor
                        radius: 3
                        implicitWidth: 200
                        implicitHeight: 3
                    }

                    Layout.fillWidth: true
                }
            }
        }
    }
}
