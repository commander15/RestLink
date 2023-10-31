import QtQuick 2.14

import RestLink 1.0

Api {
    id: api

    property string key
    property string language: "en"
    property bool adult: false
    property bool secure: false

    property int version: 3

    apiUrl: "http" + (secure ? 's' : '') + "://api.themoviedb.org/" + version

    ApiRequestParameter {
        name: "api_key"
        value: api.key
    }

    ApiRequestParameter {
        name: "language"
        value: api.language
    }

    ApiRequestParameter {
        name: "adult"
        value: (api.adult ? "true" : "false")
    }
}
