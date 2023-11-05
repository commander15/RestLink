import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

import RestLink 1.0

Page {
    id: page

    property Api api

    ColumnLayout {
        anchors.fill: parent

        TextField {
            id: endpointInput

            placeholderText: qsTr("Endpoint")
            onAccepted: request.run()

            Layout.fillWidth: true
        }

        Label {
            text: qsTr("Parameters")
            font.pointSize: endpointInput.font.pointSize
            font.bold: true
        }

        ListView {
            id: parametersView

            delegate: ApiRequestParameterDelegate {
                name: page.api.parameterName(modelData)
                value: page.api.parameterValue(modelData)
                scope: page.api.parameterScope(modelData)
                enabled: page.api.isParameterEnabled(modelData)
                width: parametersView.width
            }

            model: 0

            clip: true

            Layout.fillWidth: true
            Layout.fillHeight: true
        }

        Label {
            text: qsTr("Response")
            font.pointSize: endpointInput.font.pointSize
            font.bold: true
        }

        TextArea {
            text: JSON.stringify(request.json, null, 4);
            wrapMode: Text.WordWrap

            Layout.fillWidth: true
            Layout.fillHeight: true
        }

        RowLayout {
            Layout.fillWidth: true
            Layout.maximumHeight: 32

            Rectangle {
                color: "green"
                radius: 6

                Layout.minimumWidth: 64
                Layout.fillHeight: true

                Label {
                    id: httpCode

                    text: request.response.httpStatusCode
                    color: "white"

                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter

                    anchors.fill: parent
                }
            }

            Label {
                text: request.response.httpReasonPhrase

                verticalAlignment: Text.AlignVCenter

                Layout.fillWidth: true
                Layout.fillHeight: true
            }
        }

        Button {
            id: runButton

            text: qsTr("RUN")

            onClicked: request.run()

            Layout.alignment: Qt.AlignCenter
            Layout.bottomMargin: 12
        }
    }

    BusyIndicator {
        running: request.running

        x: page.width - width - 12
        y: runButton.y

        width: runButton.width
        height: runButton.height
    }

    ApiRequest {
        id: request

        property var json: JSON.parse(response ? response.data : "{}")

        endpoint: endpointInput.text
        api: page.api

        onFinished: console.log(response.data)
    }
}
