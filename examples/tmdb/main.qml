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

        onAccepted: request.run()

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
        autoRun: false

        onFinished: function() {
            if (response.success) {
                const rawJson = response.readBody();
                console.log(rawJson);

                var json = JSON.parse(rawJson).results;
                view.model = json;
                json = json[0];
                image.source = "http://image.tmdb.org/t/p/w500" + json.poster_path;
                label.text = "        " + json.overview;
            } else {
                console.log(response.readBody());
            }
        }

        ApiRequestParameter {
            name: "query"
            value: query.text
        }
    }

    ApiRequest {
        id: episode

        endpoint: "/tv/{id}/season/{season}/episode/{episode}"
        api: restApi

        Connections {
            target: episode.response

            function onDownloadProgress(d, t) {
                console.log(d + " / " + t);
            }

            function onFinished() {
                console.log(episode.response.readBody());
            }
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

        configurationUrl: "file:///home/commander/Downloads/TmdbConfig (1).json"

        ApiRequestParameter {
            name: "language"
            value: "fr"
        }
    }
}
