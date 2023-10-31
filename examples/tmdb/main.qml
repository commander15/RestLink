import QtQml 2.14

import QtQuick 2.14
import QtQuick.Controls 2.14
import QtQuick.Layouts 1.14

import RestLink 1.0

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

        onAccepted: request.exec()

        Layout.fillWidth: true
    }

    ListView {
        id: view

        delegate: MediaDelegate {
            poster: modelData.poster_path
            title: (modelData.title ? modelData.title : modelData.name)
            overview: modelData.overview
            popularity: modelData.popularity
            year: 2021

            accentColor: "pink"
            width: view.width
            height: 96
        }

        spacing: 5

        anchors.fill: parent
        anchors.margins: 9
    }

    ApiRequest {
        id: request

        endpoint: "/search/multi"
        api: api

        onFinished: function() {
            if (apiReply.httpStatusCode == 200) {
                var json = JSON.parse(response).results;
                view.model = json;
                json = json[0];
                image.source = "http://image.tmdb.org/t/p/w500" + json.poster_path;
                label.text = "        " + json.overview;
            }
        }

        ApiRequestParameter {
            name: "query"
            value: query.text
        }
    }

    Api {
        id: api

        apiConfigurationUrl: "https://commander-systems.000webhostapp.com/RestLink/APIs/TMDB.json"

        ApiRequestParameter {
            name: "language"
            value: "fr"
        }
    }
}
