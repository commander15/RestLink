import QtQuick 2.15
import QtQuick.Controls 2.15

import RestLink 2.0

Page {
    id: page

    required property var product
    required property Api api

    Label {
        text: page.product.name
        anchors.centerIn: parent
    }

    Request {
        endpoint: "/products/{id}"
        api: page.api

        onFinished: function() {
            if (response.success) {
                page.product = JSON.parse(response.body);
            }
        }

        RequestParameter {
            name: "id"
            value: page.product.id
            type: RequestParameter.UrlPath
        }

        RequestParameter {
            name: "with_relations"
            value: true
        }
    }
}
