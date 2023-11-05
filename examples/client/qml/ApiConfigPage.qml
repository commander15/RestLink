import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

import RestLink 1.0

Page {
    id: page

    property Api api

    title: qsTr("API Configuration")

    ColumnLayout {
        anchors.fill: parent

        TextField {
            id: nameInput
            placeholderText: qsTr("Name")

            Layout.fillWidth: true

            onTextChanged: page.api.apiName = text
        }

        TextField {
            id: versionInput
            placeholderText: qsTr("Version")

            Layout.fillWidth: true

            onTextChanged: page.api.apiVersion = parseInt(text)
        }

        TextField {
            id: urlInput
            placeholderText: qsTr("URL")

            Layout.fillWidth: true

            onTextChanged: page.api.apiUrl = text
        }

        Rectangle {
            color: "black"
            radius: 2

            Layout.fillWidth: true
            Layout.minimumHeight: 4
        }

        Label {
            text: qsTr("Parameters") + (page.api.apiParameterCount > 0 ? " (" + page.api.apiParameterCount + ')' : '')
            font.pointSize: nameInput.font.pointSize
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

            model: page.api.apiParameterCount

            clip: true

            Layout.fillWidth: true
            Layout.fillHeight: true
        }

        Rectangle {
            color: "black"
            radius: 2

            Layout.fillWidth: true
            Layout.minimumHeight: 4
        }

        TextField {
            id: configUrlInput

            text: "https://commander-systems.000webhostapp.com/RestLink/APIs/Blagues.json"
            placeholderText: qsTr("Configuration URL")

            Layout.fillWidth: true
        }

        Button {
            id: configButton

            text: qsTr("Configure")

            Layout.fillWidth: true

            onClicked: function() {
                enabled = false;
                page.api.apiConfigurationUrl = configUrlInput.text;
            }

            BusyIndicator {
                x: parent.width - width - 9
                y: (parent.height / 2) - (height / 2)
                width: parent.height - 9
                height: width

                running: !parent.enabled
            }
        }
    }

    Connections {
        target: page.api

        function onApiNameChanged(name) {
            nameInput.text = name;
        }

        function onApiVersionChanged(version) {
            versionInput.text = version;
        }

        function onApiUrlChanged(url) {
            urlInput.text = url;
        }

        function onApiConfigured() {
            configButton.enabled = true;
        }

        function onApiConfigurationFailed() {
            configButton.enabled = true;
        }
    }
}
