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
        api: restApi

        onFinished: function() {
            if (response.httpStatusCode == 200) {
                var json = JSON.parse(response.data).results;
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

    ApiRequest {
        endpoint: "/tv/{id}/season/{season}/episode/{episode}"
        api: restApi

        onFinished: function() {
            var json = JSON.parse(response.data);
            console.log(json.overview);
        }

        ApiRequestParameter {
            name: "id"
            value: 1403
            scope: ApiRequestParameter.UrlPath
        }

        ApiRequestParameter {
            name: "season"
            value: 1
            scope: ApiRequestParameter.UrlPath
        }

        ApiRequestParameter {
            name: "episode"
            value: 2
            scope: ApiRequestParameter.UrlPath
        }
    }

    Api {
        id: restApi

        apiConfigurationUrl: "https://commander-systems.000webhostapp.com/RestLink/APIs/Marvel/Discovery/TMDB3.json"

        ApiRequestParameter {
            name: "language"
            value: "fr"
        }
    }
}
