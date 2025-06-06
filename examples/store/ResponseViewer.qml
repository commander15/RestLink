import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 2.15

import RestLink 2.0

Item {
    id: page

    property Request request
    property Response response: (request ? request.response : null)

    implicitWidth: layout.implicitWidth
    implicitHeight: layout.implicitHeight

    ColumnLayout {
        id: layout

        anchors.fill: parent
        anchors.margins: 10

        ScrollView {
            Layout.fillWidth: true
            Layout.fillHeight: true

            contentWidth: availableWidth

            TextArea {
                id: responseTextArea

                readOnly: true

                font.family: "Courier New"
                font.pointSize: 12

                textFormat: TextEdit.PlainText
                wrapMode: TextArea.Wrap
            }
        }

        ProgressBar {
            id: progress
            from: 0
            visible: true
            Layout.fillWidth: true
        }
    }

    Connections {
        target: page.response

        function onDownloadProgress(downloaded, total) {
            progress.to = total;
            progress.value = downloaded;
        }

        function onFinished() {
            progress.visible = false;

            if (page.response.hasHttpStatusCode) {
                responseTextArea.text = "HTTP " + page.response.httpStatusCode + " " + page.response.httpReasonPhrase + '\n';
                var headers = page.response.headerList;
                for (var i = 0; i < headers.length; ++i) {
                    var name = headers[i].toUpperCase();
                    var value = page.response.header(headers[i]);
                    responseTextArea.text += name + ": " + value + '\n';
                }
                responseTextArea.text += "\n";

                if (page.response.header("Content-Type") === "application/json")
                    responseTextArea.text += JSON.stringify(JSON.parse(page.response.body), null, 4);
                else
                    responseTextArea.text += page.response.body;
            } else if (page.response.hasNetworkError) {
                responseTextArea.text = page.response.networkErrorString;
            } else {
                responseTextArea.text = "Unknown error occured";
            }
        }
    }
}
