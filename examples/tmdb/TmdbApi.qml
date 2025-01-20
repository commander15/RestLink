import RestLink

Api {
    id: api

    property bool adult: false
    property bool secure: false

    name: "TMDB"
    version: "3"
    url: "http" + (secure ? 's' : '') + "://api.themoviedb.org/" + version

    RequestParameter {
        name: "Authorization"
        value: api
    }

    RequestParameter {
        name: "language"
        value: api.language
    }

    RequestParameter {
        name: "adult"
        value: (api.adult ? "true" : "false")
    }
}
