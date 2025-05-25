import QtQuick 2.15
import QtQuick.Controls 2.15

import RestLink 2.0

Page {
    id: page

    required property Api api

    ListView {
        id: listView

        delegate: CategoryItemDelegate {
            product: modelData
            width: ListView.view.width
            onClicked: nameDialog.change(modelData)
        }

        spacing: 6

        anchors.fill: parent
        anchors.margins: 9

        CategoryEditDialog {
            id: nameDialog
            api: page.api
            width:  page.width * (parent.width > parent.height && parent.width >= 400 ? 0.6 : 0.9)
            anchors.centerIn: parent
            onSuccess: request.run()
        }
    }

    BusyIndicator {
        running: request.running
        anchors.centerIn: parent
    }

    Request {
        id: request

        endpoint: "/categories"
        api: page.api

        onFinished: function() {
            if (response.success) {
                var body = JSON.parse(response.body);
                listView.model = body.data;
            } else {
                console.log("HTTP " + response.httpStatusCode);
                console.log(response.body);
            }
        }

        RequestParameter {
            name: "limit"
            value: 100
        }
    }
}
