import RestLink 2.0

// This file just showcase a clean way to create a QML component on top of a RestLink one

Api {
    id: api

    property bool adult: false
    property bool secure: false

    name: "TMDB"
    version: "3"
    url: "http" + (secure ? 's' : '') + "://api.themoviedb.org/" + version

    ApiParameter {
        name: "language"
        value: "." // format: en
        locale: true // Automatically pick system locale
    }

    ApiParameter {
        name: "adult"
        value: (api.adult ? "true" : "false")
    }
}
