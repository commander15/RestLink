import QtQml 2.14

import QtQuick 2.14
import QtQuick.Controls 2.14
import QtQuick.Layouts 1.14

import RestLink

ApplicationWindow {
    id: win

    title: "TMDB"
    width: 320
    height: 640
    visible: true

    header: TextField {
        id: query

        text: "Halo"
        placeholderText: "Query..."

        onAccepted: request.run()

        Layout.fillWidth: true
    }

    ListView {
        id: view

        delegate: MediaDelegate {
            readonly property var media: model

            poster: (modelData.poster_path ? modelData.poster_path : "")
            title: (modelData.title ? modelData.title : modelData.name)
            overview: (modelData.overview ? modelData.overview : "")
            popularity: (modelData.popularity ? modelData.popularity : 0.0)
            year: 2021

            accentColor: "pink"
            width: ListView.view.width
            height: 96
        }

        spacing: 5

        anchors.fill: parent
        anchors.margins: 9
    }

    Request {
        id: request

        endpoint: "/search/multi"
        api: tmdb

        onFinished: function(response) {
            if (response.success) {
                const rawJson = response.body;
                //console.log(rawJson);

                var json = JSON.parse(rawJson).results;
                view.model = json;

                console.log(response.header('Content-Encoding'));
            } else if (response.hasHttpStatusCode) {
                console.log(response.httpReasonPhrase);
            } else if (response.hasNetworkError) {
                console.log(response.networkErrorString);
            }
        }

        RequestParameter {
            name: "query"
            value: query.text
        }
    }

    Tmdb {
        id: tmdb

        key: TMDB_API_KEY
        locale: Qt.locale("fr_FR")
    }
}
