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
        }

        spacing: 6

        anchors.fill: parent
        anchors.margins: 9
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
