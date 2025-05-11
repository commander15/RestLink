import QtQuick 2.14
import QtQuick.Controls 2.14
import QtQuick.Layouts 1.14

import RestLink 2.0

// Let's test RestLink in QML using TMDB API

ApplicationWindow {
    id: win

    // If okay, TMDB V3 is our window title
    title: tmdb.name + " V" + tmdb.version

    // Mobile like !
    width: 320
    height: 640

    // Be visible :)
    visible: true

    header: TextField {
        id: query

        text: "Halo"
        placeholderText: "Query..."

        // Run request with the new query on return
        onAccepted: request.run()
    }

    ListView {
        id: view

        delegate: MediaDelegate {
            readonly property var media: model

            // Yeah, i'm lazy, just followed TDMB API documentation ;)
            poster: (modelData.poster_path ? modelData.poster_path : "")
            title: (modelData.title ? modelData.title : modelData.name)
            overview: (modelData.overview ? modelData.overview : "")
            popularity: (modelData.popularity ? modelData.popularity : 0.0)
            year: 2025 // Something is wrong here

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

        // Just to mention, it's already the default
        method: Request.Get // Http method

        // Documentation on: https://developer.themoviedb.org
        endpoint: "/search/multi"

        // Don't forget it !
        api: tmdb

        RequestParameter {
            name: "query"
            value: query.text

            // Just to mention, it's already the default
            type: RequestParameter.UrlQuery
        }

        // We just log the full request Url
        // We do it here cause response object got created just before this signal has been emitted
        onStarted: console.log(response.url)

        // Let's understand what server said !
        onFinished: function() {
            if (response.success) {
                // Parsing JSON
                var json = JSON.parse(response.body);

                // Filling view with JSON response
                view.model = json.results;

                // Checking compression header, have you built RestLink with ZLIB ?
                // gzip and defalte supported without manual intervention
                console.log("Compression: " + response.header('Content-Encoding'));
            } else if (response.hasHttpStatusCode) {
                // Logging HTTP error
                console.log("HTTP " + response.httpStatusCode + ' ' + response.httpReasonPhrase);
            } else if (response.hasNetworkError) {
                // Logging network error
                console.log("Network Error " + response.networkError + ' ' + response.networkErrorString);
            } else {
                // Logging unknown error
                console.log("Unknown error occured");
            }
        }
    } // Request

    Api {
        id: tmdb

        name: "TMDB" // Optional
        version: "3" // Optional, yeah it's a string
        url: "https://api.themoviedb.org/3" // Mandatory

        // This will automatically add the Authorization header for you,
        // just put your Bearer token without 'Bearer' prefix
        // You can set the token in CMake cache variable of the same name so no need to
        // change something here directly, that will avoid some problems with versioning tools
        bearerToken: TMDB_BEARER_TOKEN

        // By default, system locale used, but we switch to US English okay ?
        locale: Qt.locale("en_US")

        // As a lazy man, i don't want to add these parameters on each requests
        // so i add them here for reuse.

        ApiParameter {
            name: "language"

            // value is used as format, this will use the locale Api property to generate language code
            // format examples: '.' for en, '-' for en-US and '_' for en_US
            value: "." // Here en will be generated is locale is en_US, TMDB API requirement

            // Correspond to the RestLink::Parameter::Locale flag
            locale: true // Makes the value of this property as language code taken from Api locale property
        }

        ApiParameter {
            name: "adult"
            value: false // i'm a child :)
        }

        ApiParameter {
            name: "Authorization"
            value: "Bearer ..."

            // By default, the parameter is a url query parameter, we don't want this
            type: ApiParameter.Header // Yes, it's a header !

            // We don't need it so we disable without removing, nice euh ?
            enabled: false // finally we don't need this since Api already manages it
        }
    } // Api
}

// Written with ♥️ by Amadou Wanie Benjamain !
