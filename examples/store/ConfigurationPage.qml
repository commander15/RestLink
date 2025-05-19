import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 2.15

import RestLink 2.0

Page {
    id: page

    property var configuration
    required property Api api

    StackView {
        id: stack

        initialItem: ListView {
            delegate: ItemDelegate {
                text: model.name
                width: ListView.view.width
                height: 32

                onClicked: model.action()
            }

            model: ListModel {
                ListElement {
                    name: "Endpoints"
                    action: function () {
                        stack.push(endpointListComponent)
                    }
                }

                ListElement {
                    name: "Resources"
                    action: function () {
                        console.log("Go !")
                    }
                }
            }
        }

        Component {
            id: endpointListComponent

            ColumnLayout {
                Label {
                    Layout.fillWidth: true
                }

                ListView {
                    delegate: ItemDelegate {
                        text: modelData
                        height: 64
                    }

                    model: page.configuration.endpoints.getPropertyNames

                    Layout.fillWidth: true
                    Layout.fillHeight: true
                }
            }
        }

        anchors.fill: parent
        anchors.margins: 9
    }

    Request {
        endpoint: "/configuration"
        api: page.api
        onFinished: function() {
            if (response.success) {
                page.configuration = JSON.parse(response.body);
                console.log(page.configuration);
            }
        }
    }
}
