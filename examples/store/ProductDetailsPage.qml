import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 2.15

import RestLink 2.0

Page {
    id: page

    required property var product
    required property Api api

    ColumnLayout {
        anchors.fill: parent

        ProductItemDelegate {
            product: page.product
            Layout.fillWidth: true
            Layout.bottomMargin: 12
        }

        SwipeView {
            Layout.fillWidth: true
            Layout.fillHeight: true

            ColumnLayout {
                Label {
                    text: "Stock Status"
                    font.pointSize: 14
                    font.bold: true
                    Layout.fillWidth: true
                }

                Label {
                    text: "<b>" + page.product.stock.quantity + "</b> PC(S) remaining."
                    font.pointSize: 12
                    Layout.fillWidth: true
                }

                Label {
                    text: "Last update: " + Qt.formatDateTime(page.product.stock.updated_at)
                    Layout.fillWidth: true
                }

                ResponseViewer {
                    request: pageRequest
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                }
            }
        }
    }

    Request {
        id: pageRequest

        endpoint: "/products/{id}"
        api: page.api

        onFinished: function() {
            if (response.success) {
                console.log(response.body);
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
