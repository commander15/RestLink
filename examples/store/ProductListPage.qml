import QtQuick 2.15
import QtQuick.Controls 2.15

import RestLink 2.0

Page {
    id: page

    required property Api api

    StackView {
        id: stack

        anchors.fill: parent
        anchors.margins: 9

        initialItem: ListView {
            id: listView

            delegate: ProductItemDelegate {
                product: modelData
                width: ListView.view.width
                onClicked: stack.push(detailsPage)

                Component {
                    id: detailsPage

                    ProductDetailsPage {
                        product: modelData
                        api: page.api
                    }
                }
            }

            spacing: 6

            BusyIndicator {
                running: request.running
                anchors.centerIn: parent
            }
        }
    }

    Request {
        id: request

        endpoint: "/products"
        api: page.api

        onFinished: function () {
            if (response.success) {
                var body = JSON.parse(response.body)
                listView.model = body.data
            } else {
                console.log("HTTP " + response.httpStatusCode)
                console.log(response.body)
            }
        }

        RequestParameter {
            name: "with_relations"
            value: true
        }

        RequestParameter {
            name: "limit"
            value: 100
        }
    }
}
