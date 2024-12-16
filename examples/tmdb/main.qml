import QtQml 2.14

import QtQuick 2.14
import QtQuick.Controls 2.14
import QtQuick.Layouts 1.14

import "file:///home/commander/Projects/RestLink/build/Desktop_6_8_1-Debug/qml/RestLink"

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

    ApiRequest {
        id: request

        endpoint: "/search/multi"
        api: restApi

        onFinished: function(response) {
            if (response.success) {
                const rawJson = response.readBody();
                //console.log(rawJson);

                var json = JSON.parse(rawJson).results;
                view.model = json;

                console.log(response.header('Content-Encoding'));
            }
        }

        ApiRequestParameter {
            name: "query"
            value: query.text
        }
    }

    Api {
        id: restApi

        name: "TMDB"
        //version: "3"
        url: "https://api.themoviedb.org/3"

        locale: Qt.locale("fr-FR")

        ApiRequestParameter {
            name: "api_key"
            value: TMDB_API_KEY
            scope: ApiRequestParameter.UrlQuery
        }

        ApiRequestParameter {
            name: "language"
            value: restApi.locale.name.substring(0, 2);
            scope: ApiRequestParameter.UrlQuery
        }
    }
}
